#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "avr/interrupt.h" 



RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
const byte TempButtonPin = 2;
const byte RadioBufferPin = 3;
const byte DATA_WAIT_TIME = 200;
const char CurrentMagSerialNumber[17]; // 16 length hex string. each char one of [0123456789ABCDEF]
volatile byte BulletsUsed; //todo read from eprom
volatile bool receivedSerialNumAndBulletsUsed = false;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.maskIRQ(1,1,0); 
  pinMode(TempButtonPin, INPUT);
  pinMode(RadioBufferPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(TempButtonPin), TempButtonISR, FALLING);
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


void loop() {
  if (!receivedSerialNumAndBulletsUsed)
  {
    unsigned long startMillis = millis();
    while (millis() - startMillis < DATA_WAIT_TIME) {
      if (radio.available()) {
        radio.read(&CurrentMagSerialNumber, sizeof(CurrentMagSerialNumber));
        Serial.print("2 received serial number: ");
        Serial.println(CurrentMagSerialNumber);
        break;
      }
    }
    delay(100);

    startMillis = millis();
    while (millis() - startMillis < DATA_WAIT_TIME) {
      if (radio.available()) {
        radio.read(&BulletsUsed, sizeof(BulletsUsed));
        Serial.print("4 received number bullets used: ");
        Serial.println(BulletsUsed);
        break;
      }
    }
    delay(100);
    receivedSerialNumAndBulletsUsed = true;
    attachInterrupt(digitalPinToInterrupt(RadioBufferPin), RadioBufferISR, FALLING);
  }
}



void TempButtonISR(){
  Serial.println("Enter interrupt Temp button");
  ++BulletsUsed;
  startTransmitter();
  delay(100);
  Serial.print("Send bullets used: ");
  Serial.println(BulletsUsed);
  radio.write(&BulletsUsed, sizeof(BulletsUsed));
  delay(100);
  sei();
  startReceiver();
}


void RadioBufferISR(){
  detachInterrupt(digitalPinToInterrupt(RadioBufferPin));
  receivedSerialNumAndBulletsUsed = false;
  sei();
}