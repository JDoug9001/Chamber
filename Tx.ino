#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define led 12

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
boolean buttonState = 0;

void setup() {
  pinMode(12, OUTPUT);
  radio.begin();
  radio.openWritingPipe(addresses[1]); // 00002
  radio.openReadingPipe(1, addresses[0]); // 00001
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  delay(5);
  radio.startListening();
  radio.read(&buttonState, sizeof(buttonState));
  if (buttonState == HIGH) {
  radio.stopListening();
   const char text[] = "Active";
//  radio.write(text);
  }else{
    const char text[] = "Dormant";
    radio.write(text);
    }
  
  }