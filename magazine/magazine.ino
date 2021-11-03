#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "avr/interrupt.h" 
#include "avr/sleep.h" 
#include <EEPROM.h>


RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
const int LOOP_TIME_MS = 75;
const byte MAX_NUM_BULLETS = 0x11; // 17 in decimal
const byte DEFAULT_ROUNDS_USED = 0x00;
const int ACK_WAIT_TIME = 2000;
const int LONG_PRESS_TIME = 3000;
const byte MagButtonPin = 2;
const byte RadioBufferPin = 3;
const char SerialNumber[17] = "0000000000000001"; // 16 length hex string. each char one of [0123456789ABCDEF]
volatile byte BulletsUsed;
volatile int BulletsUsedEepromAddress = 0;
volatile bool buttonPressed;


void setup() {
  Serial.begin(9600);
  getEepromBulletsUsed();
  initEeprom();
  waitForMagButtonPress();
  configurePins();
  configureInterrupts();
  configureRadio();
  goToSleep();
}

void configurePins(){
  pinMode(MagButtonPin, INPUT);
  pinMode(RadioBufferPin, INPUT);
}


void configureRadio(){
  radio.begin();
  radio.maskIRQ(1,1,0); //mask all IRQ triggers except for receive (1 is mask, 0 is no mask)
}


void configureInterrupts(){
  attachMagBttnIntrpt();
  attachRdoBffrIntrpt();
}


void attachMagBttnIntrpt(){
  attachInterrupt(digitalPinToInterrupt(MagButtonPin), MagButtonISR, FALLING);
}


void attachRdoBffrIntrpt(){
  attachInterrupt(digitalPinToInterrupt(RadioBufferPin), RadioBufferISR, FALLING);
}


void detachRdoBffrIntrpt(){
  detachInterrupt(digitalPinToInterrupt(RadioBufferPin));
}


void initEeprom(){
  // if eeprom reads out of normal range, this is a brand new chip - set the count to zero
  if (BulletsUsed > MAX_NUM_BULLETS)
  {
    setEepromBulletsUsed(DEFAULT_ROUNDS_USED);
  }
}


void setEepromBulletsUsed(byte numBulletsUsed){
  BulletsUsed = numBulletsUsed;
  Serial.print("Setting bullets used in the EEPROM to ");
  Serial.println(BulletsUsed);
  EEPROM.update(BulletsUsedEepromAddress, BulletsUsed);  
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


void transmitSerialNumber(){
  Serial.print("1 sending serial number: ");
  Serial.println(SerialNumber);
  radio.write(&SerialNumber, sizeof(SerialNumber));
}


void getEepromBulletsUsed(){
  BulletsUsed = EEPROM.read(BulletsUsedEepromAddress);
  Serial.print("Read number of bullets used from EEPROM as: ");
  Serial.println(BulletsUsed);
  return BulletsUsed;
}


void transmitNumberBulletsUsed(){
  Serial.print("3 sending number bullets used: ");
  getEepromBulletsUsed();
  radio.write(&BulletsUsed, sizeof(BulletsUsed));
}


void loop() {
  if (buttonPressed){
    // waitForAck();
    waitForMagButtonPress();
    delay(100);
    goToSleep();
  }
}


void waitForMagButtonPress(){
  buttonPressed = false;
  Serial.println("0 waiting for button press...");
}


void goToSleep(){
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Going to sleep.");
  delay(1000);
  sleep_cpu();
}


void wakeUp(){
  Serial.println("Woke up from sleep.");
  sleep_disable();
  digitalWrite(LED_BUILTIN, HIGH);
}


void receiveBulletsUsedUpdate(){
  readRadioBuffer();
  if (BulletsUsed > MAX_NUM_BULLETS) {
    getMagCount();
  } else {
    Serial.print("Received number of bullets used from the chamber: ");
    Serial.println(BulletsUsed);
    setEepromBulletsUsed(BulletsUsed);
    if (BulletsUsed == MAX_NUM_BULLETS)
    {
      Serial.println("Extend magazine ear to push slide stop up.");
    }
  }
}

void getMagCount(){
  Serial.println("Received request to send number of bullets used.");
  delay(100);
  PrepareRadioAndSendMagCount();
  delay(100);
  sei();
  Serial.println("0 waiting for button press...");
  goToSleep();
}

char readRadioBuffer(){
  radio.read(&BulletsUsed, sizeof(BulletsUsed));
  return BulletsUsed;
}


void MagButtonIsrSetup(){
  buttonPressed = true;
  Serial.println("0.5 Button Pressed");
}


bool isLongPress(){
  bool longPressed = true;
  for (int i = 0; (i * LOOP_TIME_MS) < LONG_PRESS_TIME; i++) { // while 2 seconds isnt up and not ack'd
    // Serial.println(digitalRead(MagButtonPin));
    Serial.print("milliseconds that have passed while button held: ");
    Serial.println(i * LOOP_TIME_MS);
    if (digitalRead(MagButtonPin)){ // if magButtonInput goes high, button was released
      longPressed = false;
      break;
    }
  } 
  return longPressed;
}


void handleLongPress(bool longPressed){
  if (longPressed)
  {
    Serial.println("Detected long press of mag button.");
    setEepromBulletsUsed(DEFAULT_ROUNDS_USED);
  }
}


void PrepareRadioAndSendMagCount(){
  startTransmitter();
  delay(1);
  transmitSerialNumber();
  transmitNumberBulletsUsed();
  startReceiver();
}


void MagButtonISR(){
  wakeUp();
  handleLongPress(isLongPress());
  MagButtonIsrSetup();
  PrepareRadioAndSendMagCount();
  sei();
}


void RadioBufferISR(){
  wakeUp();
  receiveBulletsUsedUpdate();
  sei();
}
