#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "avr/interrupt.h" 



RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
const byte IntPin1 = 2;
volatile byte state = LOW;

void setup() {
  pinMode (IntPin1, INPUT);
  attachInterrupt(digitalPinToInterrupt(IntPin1), MagTap_ISR, CHANGE);
  radio.begin();
  radio.openWritingPipe(addresses[1]); // 00002
  radio.setPALevel(RF24_PA_MIN);
}


void loop() {}


void MagTap_ISR(){
  state = !state;
  char text[] = state;
  radio.write(&text, sizeof(text));
  sei();
}