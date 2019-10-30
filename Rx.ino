#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>



RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};


void setup() {
  Serial.begin(9600);
    radio.begin();
  radio.openReadingPipe(1, addresses[1]); // 00002
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  delay(5);
  radio.stopListening();
    delay(5);
  radio.startListening();
 if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    delay(250);
  }
}