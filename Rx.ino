
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define button 4

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
Servo myServo;
boolean buttonState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(button, INPUT);
  radio.begin();
  radio.openWritingPipe(addresses[0]); // 00001
  radio.openReadingPipe(1, addresses[1]); // 00002
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  delay(5);
  radio.stopListening();
   buttonState = digitalRead(button);
    radio.write(&buttonState, sizeof(buttonState));
  delay(5);
  radio.startListening();
 if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    delay(1000);
  }
}