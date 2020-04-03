#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "avr/interrupt.h" 
#include <EEPROM.h>


RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
const int ACK_WAIT_TIME = 2000;
const byte MagButtonPin = 2;
const char SerialNumber[17] = "0000000000000001"; // 16 length hex string. each char one of [0123456789ABCDEF]
volatile byte BulletsUsed; // = 0;
volatile int BulletsUsedEepromAddress = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("0 waiting for button press...");
  pinMode (MagButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(MagButtonPin), MagButtonISR, FALLING);
  radio.begin();
  //todo: go to sleep
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


void transmitNumberBulletsUsed(){
  Serial.print("3 sending number bullets used: ");
  BulletsUsed = EEPROM.read(BulletsUsedEepromAddress);
  Serial.println(BulletsUsed);
  radio.write(&BulletsUsed, sizeof(BulletsUsed));
}


char* readAck(){
  char text[3] = "";
  radio.read(&text, sizeof(text));
  Serial.print("6 received ack: ");
  Serial.println(text);
  return text;
}


bool waitForAck(){
  bool acknowledged = false;
  unsigned long startMillis = millis();
  while (!acknowledged && millis() - startMillis < ACK_WAIT_TIME) { // while 2 seconds isnt up and not ack'd
    if (radio.available()) {
      if (!strcmp(readAck(), "OK")){ 
        acknowledged = true; 
        break;
      }
    }
  }
  return acknowledged;
}


void loop() {}


// todo:
// void receiveBulletsUsedUpdate(){
//   // get value from radio transmission
//   EEPROM.update(BulletsUsedEepromAddress, value)
// }


void MagButtonISR(){
  //todo: wake from  sleep
  //todo: check for long press with a while and a transmitSerialNumber
  //todo: decide how to proceed if button was long pressed
  Serial.println("0.5 Button Pressed");
  pinMode (MagButtonPin, OUTPUT);
  bool ack = false;
  startTransmitter();
  delay(1);
  transmitSerialNumber();
  transmitNumberBulletsUsed();
  startReceiver();
  ack = waitForAck();
  sei();
  pinMode (MagButtonPin, INPUT);
  Serial.println("7 back to waiting for button press...");
  //todo: if not ack'd, go back to sleep
}
