#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  digitalWrite(12, LOW); //lox1 -> 0x29
  digitalWrite(13, LOW); //lox2 -> 0x2A
  delay(10);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);

  // init lox2 
  digitalWrite(12, LOW); // disable lox1 to set I2C addy of lox2
  if (!lox2.begin(0x2A)) { //init lox2 with 0x2A I2C addy
    Serial.println(F("Failed to boot VL53L0X_2"));
    while(1);
  }
  digitalWrite(12, HIGH); // enable lox1; done setting lox2 I2C addy

  // init lox1
  if (!lox1.begin()) { //init lox2 with 0x2A I2C addy
    Serial.println(F("Failed to boot VL53L0X_1"));
    while(1);
  }
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
