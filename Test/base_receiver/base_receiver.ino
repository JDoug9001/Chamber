#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "avr/interrupt.h" 



RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
const char Acknowledge[3] = "OK";
const char CurrentMagSerialNumber[17]; // 16 length hex string. each char one of [0123456789ABCDEF]
volatile byte BulletsUsed; //todo read from eprom

void setup() {
  Serial.begin(9600);
  radio.begin();
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
  startReceiver();
  while (true) {
    if (radio.available()) {
      radio.read(&CurrentMagSerialNumber, sizeof(CurrentMagSerialNumber));
      Serial.print("2 received serial number: ");
      Serial.println(CurrentMagSerialNumber);
      break;
    }
  }

  while (true) {
    if (radio.available()) {
      radio.read(&BulletsUsed, sizeof(BulletsUsed));
      Serial.print("4 received number bullets used: ");
      Serial.println(BulletsUsed);
      break;
    }
  }
  startTransmitter();
  Serial.print("5 sending ack: ");
  Serial.println(Acknowledge);
  radio.write(&Acknowledge, sizeof(Acknowledge));
}
