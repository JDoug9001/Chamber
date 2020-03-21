#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "avr/interrupt.h" 



RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
const byte IntPin1 = 2;
const char SerialNumber[17] = "0000000000000001"; // 16 length hex string. each char one of [0123456789ABCDEF]
volatile byte BulletsUsed = 0; //todo read from eprom

void setup() {
  Serial.begin(9600);
  Serial.println("0 waiting for button press...");
  pinMode (IntPin1, INPUT);
  attachInterrupt(digitalPinToInterrupt(IntPin1), MagButtonISR, FALLING);
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
  pinMode (IntPin1, OUTPUT);
  bool acknowledged = false;
  startTransmitter();
  delay(1);
  Serial.print("1 sending serial number: ");
  Serial.println(SerialNumber);
  radio.write(&SerialNumber, sizeof(SerialNumber));

  Serial.print("3 sending number bullets used: ");
  Serial.println(BulletsUsed);
  radio.write(&BulletsUsed, sizeof(BulletsUsed));
  //delay(200);
  startReceiver();

  while (true) {
    if (radio.available()) {
      char text[3] = "";
      radio.read(&text, sizeof(text));
      Serial.print("6 received ack: ");
      Serial.println(text);
      if (!strcmp(text, "OK")){
        acknowledged = true;
        break;
      }
    }
  }
  // other stuff later
  sei();
  pinMode (IntPin1, INPUT);
  Serial.println("7 back to waiting for button press...");
}
