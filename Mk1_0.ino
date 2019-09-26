#include "avr/interrupt.h" 
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

volatile int value=0;
const byte Solenoid = 13;
const byte IntPin1 = 2;
const byte IntPin2 = 3;
const byte IntPin3 =4;
volatile byte state = 0;
const byte SwitchFunctionButton = 10;
volatile byte MagTof = 0;
volatile byte SlideTof = 1;
int VibraSensPin = 6;

void setup() {

  pinMode (solenoid, OUTPUT);
  pinMode (IntPin1, INPUT_PULLUP);
  pinMode (IntPin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IntPin1), Prox_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(IntPin2), Mode_ISR, RISING);
  attachInterrupt(digitaPinToInterrupt(IntPin3), Vibra_ISR, CHANGE); 
  // Variables 

  int ProxCounter = 1; // counter for the number of Prox Sensor        
  int LastProxState = 0; // previous state of the Prox Sensor
  int Reload = 0; //Initiate Reload State 
  int RandomInt = 0; //Random number for malfunction
  int PrevRandInt = 0; //Previous random number to prevent repeated value
  int Malfunction = 0; //Malfuntion function  
  int SwitchFunctionButton_State = 1; //Button State for switching functions
  int Timeout = 0; //
  int Mag_Distance = 20;
   int VibraSens = 0; 
  
  // Inputs and Outputs

  pinMode (ProxState, INPUT_PULLUP); //current state of the Prox Sensor
  pinMode (MagButton, INPUT); //Momentary push for mag "tap"
  pinMode (SlideTof, INPUT); //SlideTOF sensor for distance shows slide distance from device
  pinMode (MagTof_Remove, INPUT); //value for removing MagTof
  pinMode (MagTof_Insert, INPUT); //value for Inserting MagTof Read the same pin for both magtof remove and insert
  pinMode (Solenoid, OUTPUT); //Solenoid to push the slide
  pinMode (SwitchFunctionButton, INPUT); //Button to switch between the three functions
  pinMode (Blue_Led_1, OUTPUT); //LED to indicate which function is being run
  pinMode (Blue_Led_2, OUTPUT); //LED to indicate which function is being run
  pinMode (Blue_Led_3, OUTPUT); //LED to indicate which function is being run
  pinMode (RGB_Led, OUTPUT); //LED to indicate power level 
  pinMode (VibraSens, INPUT); //Motion switch to turn on 

  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  //Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 

} 
}



void loop() {


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
  }
  sei();
}

void Mode_ISR(){
  //Button to switch between programs
 //If a button is pressed then the device will cycle through its 3 modes
  SwitchFunctionButton_State++
  if(SwitchFunctionButton_State > 3){
    SwitchFunctionButton_State = 1;
  }
  
  switch (SwitchFunctionButton_State)
  {
    case 1: // code to be executed if SwitchFunctionButton_State = 1;
      Blue_Led_1 = HIGH;
      break;
    case 2: // code to be executed if SwitchFunctionButton_State = 2;
      Blue_Led_1 = HIGH;
      Blue_Led_2 = HIGH; 
     
      break;
    case 3: // code to be executed if SwitchFunctionButton_State = 3;
      Blue_Led_1 = HIGH;
      Blue_Led_2 = HIGH;
      Blue_Led_3 = HIGH;
      
      break;   
    default: // code to be executed if n doesn't match any cases
  }
  sei();
}

void Reload_Mode(){
  if (ProxCounter == 15) {
    Reload = 1;
    if (Reload == 1){ //Tof Sensor on top of Mag compares distance value. When the distance increases (the mag has been dropped) the loop finds a new number, ProxCounter resets. When the distance decreases (the mag has been reinserted) the reload is reset to zero. 
      Distance = Get_Tof_Dist(MagTof);
      for (Mag_Distance > Distance){
        PrevRandomInt = RandInt; //Find a new random number ^look up at random function^
        ProxCounter = 1; //Reset the count to zero
        Get_Tof_Dist(MagTof);
        if(Mag_Distance<xxx distance)
          Reload = 0; //Reset Main interrupt 
  }
  }
  Cycle_Slide();
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
    if (Malfunction = 1){ //If Momentary is pushed then Tof reaches certain distance go high then malfunction is reset to zero and then the cycle will continue to loop until 15 actuations are reached
    digitalRead (MagButton); //"Mag" is tapped
      if(MagButton == HIGH){
      Get_Mag_Tof (SlideTof);
        if (SlideTof> XXX){  //Tof sensor says slide was pulled back X distance ie racked. Solenoid is turned off 
        Malfunction = 0;     //Malfunction is cleared
}
} 
}
}
}
  Reload_Mode();
}

void Cycle_Slide(){
  if(Malfunction != 1)&&(Reload != 1){
    digitalWrite(Solenoid, HIGH);
    delay(500);
    digitalWrite(Solenoid, LOW);
    delay(500);
  } 
}


int Get_Tof_Dist(int sensor) {
  VL53L0X_RangingMeasurementData_t measure;
    
  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }
    
  delay(100);
}

//Wireless signal transmission/receiveing,  deepsleep and wake interrupt



//Code for transmitter
/*
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN         
const byte address[6] = "00001";     //Byte of array representing the address. This is the address where we will send the data. This should be same on the receiving side.
int button_pin = 2;
boolean button_state = 0;
void setup() {
pinMode(button_pin, INPUT);
radio.begin();                  //Starting the Wireless communication
radio.openWritingPipe(address); //Setting the address where we will send the data
radio.setPALevel(RF24_PA_MIN);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
radio.stopListening();          //This sets the module as transmitter
}
void loop()
{
button_state = digitalRead(button_pin);
if(button_state == HIGH)
{
const char text[] = "Your Button State is HIGH";
radio.write(&text, sizeof(text));                  //Sending the message to receiver
}
else
{
const char text[] = "Your Button State is LOW";
radio.write(&text, sizeof(text));                  //Sending the message to receiver 
}
radio.write(&button_state, sizeof(button_state));  //Sending the message to receiver 
delay(1000);
}
*/

//Code for receiver
/*
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";
boolean button_state = 0;
int led_pin = 3;
void setup() {
pinMode(6, OUTPUT);
Serial.begin(9600);
radio.begin();
radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
radio.startListening();              //This sets the module as receiver
}
void loop()
{
if (radio.available())              //Looking for the data.
{
char text[32] = "";                 //Saving the incoming data
radio.read(&text, sizeof(text));    //Reading the data
radio.read(&button_state, sizeof(button_state));    //Reading the data
if(button_state == HIGH)
{
digitalWrite(6, HIGH);
Serial.println(text);
}
else
{
digitalWrite(6, LOW);
Serial.println(text);}
}
delay(5);
} 
*/

// https://create.arduino.cc/projecthub/muhammad-aqib/nrf24l01-interfacing-with-arduino-wireless-communication-0c13d4