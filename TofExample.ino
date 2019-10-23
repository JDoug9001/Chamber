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

  // init lox2 
  lox2.init(true);
  lox2.setAddress(0x2A);
  // if (!lox2.begin(0x2A)) { //init lox2 with 0x2A I2C addy
  //   Serial.println(F("Failed to boot VL53L0X_2"));
  //   while(1);
  // }
  delay(10);
  digitalWrite(12, HIGH); // enable lox1; done setting lox2 I2C addy

  // init lox1
  // if (!lox1.begin()) { //init lox2 with 0x2A I2C addy
  //   Serial.println(F("Failed to boot VL53L0X_1"));
  //   while(1);
  // }
  lox1.init(true);
  //lox1.setAddress(0x2A);

  // power 
  Serial.println(F("VL53L0X API Simple Ranging example  using two sensors.\n\n")); 
}


void loop() {
  VL53L0X_RangingMeasurementData_t measure1;
  VL53L0X_RangingMeasurementData_t measure2;
    
  Serial.print("Reading lox1 measurement... ");
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  if (measure1.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure1.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }

  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!
  if (measure2.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure2.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }
    
  delay(100);
}
