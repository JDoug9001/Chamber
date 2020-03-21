#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "avr/interrupt.h" 



RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
const byte IntPin1 = 2;
const string SerialNumber = "0000000000000001" // 16 length hex string. each char one of [0123456789ABCDEF]
volatile byte BulletsUsed = 0; //todo read from eprom

void setup() {
  pinMode (IntPin1, INPUT);
  attachInterrupt(digitalPinToInterrupt(IntPin1), MagButtonISR, CHANGE);
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


void loop() {}


void MagButtonISR(){
  bool acknowledged = false;
  startTransmitter();
  radio.write(&SerialNumber, sizeof(SerialNumber));
  radio.write(&BulletsUsed, sizeof(BulletsUsed));
  startReceiver();

  while (true) {
    if (radio.available()) {
      char text[32] = "";
      radio.read(&text, sizeof(text));
      Serial.println("got response of " + text);
      if (text == 'OK'){
        acknowledged = true;
        break;
      }
    }
  }
  // other stuff later
  sei();
}
