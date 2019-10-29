#include <Wire.h>
#include "VL53L0X.h"

VL53L0X lox1;
VL53L0X lox2;

void setup() {
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW); //lox1 -> 0x29
  Wire.begin();
  Serial.begin(115200);
  
  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  delay(500);
  lox2.init(true);
  lox2.setAddress(0x2A);
  delay(10);
  digitalWrite(12, HIGH); // enable lox1; done setting lox2 I2C addy
  lox1.init(true);
  Serial.println(F("VL53L0X API Simple Ranging example  using two sensors.\n\n")); 
}


void loop() {
  Serial.print("Reading lox1 measurement... ");
  Serial.print("Distance (mm): "); Serial.println(lox1.readRangeSingleMillimeters());

  Serial.print("Reading lox2 measurement... ");
  Serial.print("Distance (mm): "); Serial.println(lox2.readRangeSingleMillimeters());
    
  delay(100);
}
