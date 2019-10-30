#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define button 4

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
boolean buttonState = 0;

void setup() {
  pinMode(button, INPUT);
  radio.begin();
  radio.openWritingPipe(addresses[1]); // 00002
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  delay(5);
   char text[8]= "" ;
   buttonState = digitalRead(button);
  if (buttonState == HIGH) {
    strcpy(text, "Active");
   }else{
  strcpy(text, "Dormant");
    
    }
  radio.write(&text,sizeof(text));
  }