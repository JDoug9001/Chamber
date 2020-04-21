#include "avr/interrupt.h" 
// #include "VL53L0X.h"
#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// N O T E S 
// store magazine name and current_bullets_used in memory
// set count to the current_bullets_used from the mag
// tap_rack_bang()
// (dont talk to other mags until current mag empty/unresponsive)

// C O N S T A N T S
const byte NUM_BULLETS = 17;
const int SHORT_NUM_STEPS = 1;
const int LONG_NUM_STEPS = 2;
const int TAP_RACK_BANG_TIME = 2; // seconds
const int TRB_WAIT_INTERVAL = 10; // miliseconds
const byte DATA_WAIT_TIME = 200;
const byte address[6] = "00001";
const byte TempButtonPin = 2; // button to act as prox sensor input
const byte RadioBufferPin = 3;
const byte led1 = 4;
const byte led2 = 5;
const byte led3 = 6;
const char CurrentMagSerialNumber[17]; // 16 length hex string. each char one of [0123456789ABCDEF]

// G L O B A L S
RF24 radio(7, 8); // CE, CSN
volatile byte mode = 1;
volatile int BulletsUsed = 0;
volatile bool jammed = false;
volatile bool new_magazine = false;
volatile bool receivedSerialNumAndBulletsUsed = false;
volatile bool limit_switch = true; // impersonate the limnit switch input
volatile bool slide_fully_extended = true; // impersonate the slide fully extended input


void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.maskIRQ(1,1,0); 
  pinMode(TempButtonPin, INPUT);
  pinMode(RadioBufferPin, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(TempButtonPin), prox_sensor_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RadioBufferPin), RadioBufferISR, FALLING);
  startReceiver();
}


void startReceiver(){
  radio.openReadingPipe(0, address);;
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}


void startTransmitter(){
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}


void continuous_mode_ops(){
  // Motor turns on for short encoder steps/time
  drive_motor(SHORT_NUM_STEPS);
  return_home();
}


void reload_mode_ops() {
  if (BulletsUsed < NUM_BULLETS){
    continuous_mode_ops();
  } else {
    // motor turn long encoder steps
    drive_motor(LONG_NUM_STEPS);
    
    /* 
    // might need to wait until mag has extended the follower before return slide to home
    // need to experiment to find out for sure
    while (!mag_follower_extended()){
      delay(1);
    } 
    */

    // (slide locked open at this point -- this is done on the mag side)
    return_home();

    // wait forever for a new magazine greeting interrupt
    while (!new_magazine){
      delay(500);
    } 

    // made it past the wait for new mag. now, set new mag to false
    new_magazine = false;
  }
}


void drive_motor(int num_steps){
  // TODO: figure out what this looks like once we get an encoder
}


void return_home(){
  // over and over forever
  while (true) {
    // turn backward one step
    drive_motor(-1);
    // if it hits the limit switch
    if (limit_switch){
      // bail out of loop
      break;
    }
  }
}


void rand_mode_ops(){
  int RandomInt = random(NUM_BULLETS + 1); //Random number between 0-NUM_BULLETS
  if (BulletsUsed != RandomInt){
    reload_mode_ops();
  }
  else {
    jammed = true;

    // this looks really funny here but it is legit. when the mag button is pressed, 
    // the same message is sent to the chamber regardless of whether it was a 
    // 'tap-rack-bang' button press or a reload button press
    
    // wait forever for tap-rack-bang
    while (jammed){
      delay(10);
    } 
  }
}


void tap_rack_bang(){
  // wait 'wait_seconds' for dist sensor to show slide has been fully extended
  float max_i = TAP_RACK_BANG_TIME*1000 / TRB_WAIT_INTERVAL;
  for (int i = 0; i < max_i; i++){
    delay(10);
    if (slide_fully_extended){
      jammed = false;
      break;
    }
  }
}


void set_mag_count(){
  Serial.println("Enter interrupt Temp button");
  ++BulletsUsed; // add one to number of bullets used
  startTransmitter();
  delay(100);
  Serial.print("Send bullets used: ");
  Serial.println(BulletsUsed);
  radio.write(&BulletsUsed, sizeof(BulletsUsed));
  delay(100);
  sei();
  startReceiver();
}


void waitForNumBullets(){
  unsigned long startMillis = millis();
  while (millis() - startMillis < DATA_WAIT_TIME) {
    if (radio.available()) {
      radio.read(&BulletsUsed, sizeof(BulletsUsed));
      Serial.print("4 received number bullets used: ");
      Serial.println(BulletsUsed);
      break;
    }
  }
}


void waitForSerialNum(){
  unsigned long startMillis = millis();
  while (millis() - startMillis < DATA_WAIT_TIME) {
    if (radio.available()) {
      radio.read(&CurrentMagSerialNumber, sizeof(CurrentMagSerialNumber));
      Serial.print("2 received serial number: ");
      Serial.println(CurrentMagSerialNumber);
      break;
    }
  }
}


void loop() {
  if (!receivedSerialNumAndBulletsUsed)
  {
    waitForSerialNum();
    delay(100);
    waitForNumBullets();
    delay(100);
    receivedSerialNumAndBulletsUsed = true;
    attachInterrupt(digitalPinToInterrupt(RadioBufferPin), RadioBufferISR, FALLING);
    new_magazine = true;
  }
}


void RadioBufferISR(){
  detachInterrupt(digitalPinToInterrupt(RadioBufferPin));
  receivedSerialNumAndBulletsUsed = false;
  sei();
}


// prox sensor trigger this ISR
void prox_sensor_ISR(){
  set_mag_count();
  switch (mode){
    case 1:
      continuous_mode_ops();
      break;
    case 2:
      reload_mode_ops();
      break;
    case 3:
      rand_mode_ops();
      break;
  }
}


// todo: use an interrupt on change interrupt for this on a pin other than 2 or 3
void mode_button_ISR(){
  switch (++mode){
    case 2:
      // light the 2nd LED
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      break;
    case 3:
      // light the 3rd LED
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, HIGH);
      break;
    default:
      mode = 1;
      // light the 1st LED
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
  }
}