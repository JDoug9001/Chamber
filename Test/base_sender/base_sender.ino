#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "avr/interrupt.h" 
#include "avr/sleep.h" 
#include <EEPROM.h>


RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
const int SLEEP_TIME_MS = 50;
const int ACK_WAIT_TIME = 2000;
const int LONG_PRESS_TIME = 3000;
const byte MagButtonPin = 2;
const byte RadioBufferPin = 3;
const char SerialNumber[17] = "0000000000000001"; // 16 length hex string. each char one of [0123456789ABCDEF]
volatile byte BulletsUsed;
volatile int BulletsUsedEepromAddress = 0;
volatile bool acknowledged = true;
volatile bool buttonPressed;


void setup() {
  Serial.begin(9600);
  getEepromBulletsUsed();
  initEeprom();
  waitForMagButtonPress();
  pinMode(RadioBufferPin, INPUT);
  configureInterrupts();
  configureRadio();
  // goToSleep();
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
  // if eeprom reads 255, this is a brand new chip - set the count to zero
  if (BulletsUsed == 0xFF)
  {
    setEepromBulletsUsed(0);
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


char* readAck(){
  char text[3] = "";
  radio.read(&text, sizeof(text));
  Serial.print("6 received ack: ");
  Serial.println(text);
  return text;
}


void waitForAck(){
  detachRdoBffrIntrpt();
  unsigned long startMillis = millis();
  while (!acknowledged && millis() - startMillis < ACK_WAIT_TIME) { // while 2 seconds isnt up and not ack'd
    if (radio.available()) {
      if (strcmp(readAck(), "OK")){ 
        acknowledged = true; 
        break;
      }
    }
  }
  attachRdoBffrIntrpt();
}


void loop() {
  if (buttonPressed && !acknowledged){
    waitForAck();
    waitForMagButtonPress();
    if (!acknowledged){
      // goToSleep();
    }
  }
}


void waitForMagButtonPress(){
  buttonPressed = false;
  pinMode (MagButtonPin, INPUT);
  Serial.println("0 waiting for button press...");
}


void goToSleep(){
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  Serial.println("Going to sleep.");
  sleep_cpu();
}


void wakeUp(){
  Serial.println("Woke up from sleep.");
  sleep_disable();
  digitalWrite(LED_BUILTIN, HIGH);
}


void receiveBulletsUsedUpdate(){
  char value = readRadioBuffer();
  Serial.print("Received number of bullets used from the chamber: ");
  Serial.println(value);
  EEPROM.update(BulletsUsedEepromAddress, value);
}


char readRadioBuffer(){
  char* val[1];
  radio.read(&val, sizeof(val));
  return val;
}


void interruptSetup(){
  buttonPressed = true;
  Serial.println("0.5 Button Pressed");
  pinMode (MagButtonPin, OUTPUT);
  acknowledged = false;
}


bool isLongPress(){
  bool longPressed = true;
  for (int i = 0; (i * SLEEP_TIME_MS) < LONG_PRESS_TIME; i++) { // while 2 seconds isnt up and not ack'd
    delay(SLEEP_TIME_MS);
    // if magButtonInput goes high, button was released
    if (digitalRead(MagButtonPin)){
      longPressed = false;
      break;
    }
    Serial.print("milliseconds that have passed while button held: ");
    Serial.println(i * SLEEP_TIME_MS);
  } 
  return longPressed;
}


void handleLongPress(bool longPressed){
  if (longPressed)
  {
    Serial.println("Detected long press of mag button.");
    setEepromBulletsUsed(0);
  }
}


void MagButtonISR(){
  // wakeUp();
  handleLongPress(isLongPress());
  interruptSetup();
  startTransmitter();
  delay(1);
  transmitSerialNumber();
  transmitNumberBulletsUsed();
  startReceiver();
  sei();
}


void RadioBufferISR(){
  // wakeUp();
  receiveBulletsUsedUpdate();
  sei();
}