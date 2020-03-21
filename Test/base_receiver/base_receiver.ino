#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "avr/interrupt.h" 



RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
const byte Acknowledge[3] = "OK"
volatile byte CurrentMagSerialNumber[17]; // 16 length hex string. each char one of [0123456789ABCDEF]
volatile byte BulletsUsed; //todo read from eprom

void setup() {
  pinMode (IntPin1, INPUT);
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
      Serial.println("got serial number of " + text);
      break;
    }
  }

  while (true) {
    if (radio.available()) {
      radio.read(&BulletsUsed, sizeof(BulletsUsed));
      Serial.println("got number bullets used " + text);
      break;
    }
  }
  startTransmitter();
  Serial.println("sending ack " + Acknowledge);
  radio.write(&Acknowledge, sizeof(Acknowledge));
}
