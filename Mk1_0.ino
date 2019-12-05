#include "avr/interrupt.h" 
#include "VL53L0X.h"
#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>




VL53L0X SlideTof = VL53L0X();
VL53L0X MagazineTof = VL53L0X();

// wireless modules
// https://create.arduino.cc/projecthub/muhammad-aqib/nrf24l01-interfacing-with-arduino-wireless-communication-0c13d4
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

const byte Solenoid = 10; //this is referencing the slide solenoid used for normal operation
//const byte MagSolenoid = 9;  //This solenoid extends during reload sequence
const byte ProxSensorPin = 1;
const byte ModeBtnPin = 3;
const byte MagTofXShutPin = 5;
//volatile int WakeSensPin = 4;
volatile int Blue_Led_1=0;
volatile int Blue_Led_2=6;
volatile int Blue_Led_3=9;
volatile byte MagButton = 0;
volatile byte ProxCounter = 1; // counter for the number of Prox Sensor        
volatile byte SwitchFunctionButton_State = 1; //Button State for switching functions
volatile byte LastProxState = 0; // previous state of the Prox Sensor
volatile byte Reload = 0; //Initiate Reload State 
volatile int RandomInt = 0; //Random number for malfunction
volatile  int PrevRandInt = 0; //Previous random number to prevent repeated value
volatile byte Malfunction = 0; //Malfuntion function  
  //int Timeout = 0; //
 const byte Mag_Distance = 20;
  //int VibraSens = 0; 


//

int Get_Tof_Dist(VL53L0X sensor) {
  Serial.print("Reading a measurement... ");
  int dist = sensor.readRangeSingleMillimeters();
  // delay(100);
  return dist;
}

void Cycle_Slide(){
  if ((Malfunction != 1) && (Reload != 1)) {
    digitalWrite(Solenoid, HIGH);
    delay(500);
    digitalWrite(Solenoid, LOW);
    delay(500);
  } 
}

void Reload_Mode(){
  if (ProxCounter == 15) {
    Reload = 1;
 //   digitalWrite(MagSolenoid, HIGH); //Extend mag follower solenoid
    if (Reload == 1){ //Tof Sensor on top of Mag compares distance value. When the distance increases (the mag has been dropped) the loop finds a new number, ProxCounter resets. When the distance decreases (the mag has been reinserted) the reload is reset to zero. 
      
      double Distance = Get_Tof_Dist(MagazineTof);
      while (Distance < Mag_Distance){ // mag still in wait until it gets pulled
        delay(100);
        Distance = Get_Tof_Dist(MagazineTof);
      }

      delay(100);
      Distance = Get_Tof_Dist(MagazineTof);
      while (Distance > Mag_Distance){ // mag out, wait until it is replaced
        delay(100);
        Distance = Get_Tof_Dist(MagazineTof);
      }
      ProxCounter = 1; //Reset the count to zero
      Reload = 0; //Reset Main interrupt 
//      digitalWrite(MagSolenoid, LOW); //Retract mag follower
    }
  } else {
    Cycle_Slide();
  }
}


void Random_Mode(){
  //Generate a random number. If it's that same as the previous number, generate a new number
  RandomInt = random(16); //Random number between 0-15
  while(RandomInt == PrevRandInt){
    RandomInt = random(16); //Random number between 0-15
  }
  //Stop main interrupt from functioning when the random number and counter number are the same
  if (ProxCounter == RandomInt){
    Malfunction = 1;    //In main interrupts Malfunction!=1
    while (Malfunction == 1){ //If Momentary is pushed then Tof reaches certain distance go high then malfunction is reset to zero and then the cycle will continue to loop until 15 actuations are reached
      if (MagButton == HIGH){
        // tapped
        double Distance = Get_Tof_Dist(SlideTof);
        while (Distance < 100){  //Tof sensor says slide was pulled back X distance ie racked. Solenoid is turned off 
          delay(100);
          Distance = Get_Tof_Dist(SlideTof);
        }
        // racked
        Malfunction = 0;  //Malfunction is cleared
      } 
    }
  }
  Reload_Mode();
  PrevRandInt = RandomInt; //Find a new random number ^look up at random function^
}





//
void setup() {
  // wireless module
  radio.begin();
  radio.openReadingPipe(1, addresses[1]); // 00002
  radio.setPALevel(RF24_PA_MIN);

  //Tof setup start
  pinMode(MagTofXShutPin, OUTPUT);
  digitalWrite(MagTofXShutPin, LOW); //MagazineTof -> 0x29
  Wire.begin();
  Serial.begin(115200);
  
  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  delay(500);
  SlideTof.init(true);
  SlideTof.setAddress(0x2A);
  delay(10);
  digitalWrite(MagTofXShutPin, HIGH); // enable MagazineTof; done setting SlideTof I2C addy
  MagazineTof.init(true);
  Serial.println(F("VL53L0X API Simple Ranging example  using two sensors.\n\n")); 
//End Tof setup

  pinMode (Solenoid, OUTPUT);
//  pinMode (MagSolenoid, OUTPUT);
  pinMode (ProxSensorPin, INPUT_PULLUP);
  pinMode (ModeBtnPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ProxSensorPin), Prox_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(ModeBtnPin), Mode_ISR, RISING);
//  attachInterrupt(digitalPinToInterrupt(WakeSensorPin), Vibra_ISR, CHANGE); 
  // Variables 

  
  
  // Inputs and Outputs

//  pinMode (ProxState, INPUT_PULLUP); //current state of the Prox Sensor
  pinMode (Blue_Led_1, OUTPUT); //LED to indicate which function is being run
  pinMode (Blue_Led_2, OUTPUT); //LED to indicate which function is being run
  pinMode (Blue_Led_3, OUTPUT); //LED to indicate which function is being run
//  pinMode (RGB_Led, OUTPUT); //LED to indicate power level 
//  pinMode (VibraSens, INPUT); //Motion switch to turn on 
} 


void loop() {
  delay(5);
  radio.stopListening();
  delay(5);
  radio.startListening();
 if (radio.available()) {
    char text[2] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    MagButton = text[0];
    delay(1000);
  }
}


void Prox_ISR(){
  ProxCounter++;
  switch (SwitchFunctionButton_State)
  {
    case 1: // code to be executed if SwitchFunctionButton_State = 1;
      Cycle_Slide();
      break;
    case 2: // code to be executed if SwitchFunctionButton_State = 2;
      Reload_Mode(); 
      break;
    case 3: // code to be executed if SwitchFunctionButton_State = 3;
      Random_Mode();
      break;   
    default: // code to be executed if n doesn't match any cases
      break;
  }
  sei();
}

void Mode_ISR(){
  //Button to switch between programs
 //If a button is pressed then the device will cycle through its 3 modes
  SwitchFunctionButton_State++;
  if(SwitchFunctionButton_State > 3){
    SwitchFunctionButton_State = 1;
  }
  
  switch (SwitchFunctionButton_State)
  {
    case 1: // code to be executed if SwitchFunctionButton_State = 1;
      Blue_Led_1 = HIGH;
      Blue_Led_2 = LOW;
      Blue_Led_3 = LOW;
      break;
    case 2: // code to be executed if SwitchFunctionButton_State = 2;
      Blue_Led_1 = HIGH;
      Blue_Led_2 = HIGH; 
      Blue_Led_3 = LOW;
      break;
    case 3: // code to be executed if SwitchFunctionButton_State = 3;
      Blue_Led_1 = HIGH;
      Blue_Led_2 = HIGH;
      Blue_Led_3 = HIGH;
      
      break;   
    default:  // code to be executed if n doesn't match any cases
      break;
  }
  sei();
}

