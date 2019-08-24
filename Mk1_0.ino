#include "avr/interrupt.h" 

volatile int value=0;
const byte Solenoid = 13;
const byte IntPin1 = 2;
const byte IntPin2 = 3;
volatile byte state = 0;
const byte SwitchFunctionButton = 10;
//Testing 1 2 3

void setup() {

  pinMode (solenoid, OUTPUT);
  pinMode (IntPin1, INPUT_PULLUP);
  pinMode (IntPin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IntPin1), Prox_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(IntPin2), Mode_ISR, RISING);
  
  // Variables 

  int ProxCounter = 0; // counter for the number of Prox Sensor        
  int LastProxState = 0; // previous state of the Prox Sensor
  int Reload = 0; //Initiate Reload State 
  int RandomInt = 0; //Random number for malfunction
  int PrevRandInt = 0; //Previous random number to prevent repeated value
  int Malfunction = 0; //Malfuntion function  
  int SwitchFunctionButton_State = 1; //Button State for switching functions
  int Timeout = 0; //


  // Inputs and Outputs

  pinMode (ProxState, INPUT_PULLUP); //current state of the Prox Sensor
  pinMode (MagButton, INPUT); //Momentary push for mag "tap"
  pinMode (SlideTof, INPUT); //SlideTOF sensor for distance shows slide distance from device
  pinMode (MagTof_OldValue, INPUT); //Old value for MagTof
  pinMode (Solenoid, OUTPUT); //Solenoid to push the slide
  pinMode (SwitchFunctionButton, INPUT); //Button to switch between the three functions
  pinMode (Blue_Led_1, OUTPUT); //LED to indicate which function is being run
  pinMode (Blue_Led_2, OUTPUT); //LED to indicate which function is being run
  pinMode (Blue_Led_3, OUTPUT); //LED to indicate which function is being run
  pinMode (RGB_Led, OUTPUT); //LED to indicate power level 
  pinMode (VibrationSens, INPUT_PULLUP); //Motion switch to turn on 
}


//Deep sleep
// Interrupt for turning on device goes here set Timeout = 1. 
// If motion sensor isnt hit again within X time limit. timeout resets to zero. Program exits the loop

 


void loop() {
  //Button to switch between programs
  if(digitalRead(SwitchFunctionButton) == HIGH){ //If a button is pressed then the device will cycle through its 3 modes
    SwitchFunctionButton_State++
    if(SwitchFunctionButton_State > 3){
      SwitchFunctionButton_State = 1;
    }
  }
  switch (SwitchFunctionButton_State)
  {
    case 1: // code to be executed if SwitchFunctionButton_State = 1;
      Blue_Led_1 = HIGH;
      break;
    case 2: // code to be executed if SwitchFunctionButton_State = 2;
      Blue_Led_1 = HIGH;
      Blue_Led_2 = HIGH; 
      Reload_Function(); 
      break;
    case 3: // code to be executed if SwitchFunctionButton_State = 3;
      Blue_Led_1 = HIGH;
      Blue_Led_2 = HIGH;
      Blue_Led_3 = HIGH;
      Random_Function();
      break;   
    default: // code to be executed if n doesn't match any cases
  }

  if((value==1)&&(Malfunction !=1)&&(Reload !=1){
    digitalWrite(Solenoid, HIGH);
    delay(500);
    digitalWrite(Solenoid, LOW);
    delay(500);

    value=0;
  }
}

void Prox_ISR(){
  ProxCounter++;
  value = 1;
  sei();
}

void Mode_ISR(){
  ProxCounter++;
  value = 1;
  sei();
}

void Reload_Function(){
  if (ProxCounter == 15) {
}
}


void Random_Function(){
 
}



 










//Reload Function Called
  if ((SwitchFunctionButton==2)&&(Reload!=1)){

Blue_Led_1 = HIGH;
Blue_Led_2 = HIGH;

// Detect prox sensor going high and count number of occurences 
  if (ProxState = RISING) {  
      ProxCounter++;
  }
// interrupt main interrupt; //stops cycling.....Nest an if loop into interrupt function if("Reload!=1")
  if (ProxCounter == 15){
      Reload = 1;
      } 

  if (Reload == 1){ //Tof Sensor on top of Mag compares distance value. When the distance increases (the mag has been dropped) the loop finds a new number, ProxCounter resets. When the distance decreases (the mag has been reinserted) the reload is reset to zero. 
      int MagTof_NewValue = analogRead(A?); //Read the input on analog pin
          if(MagTof_NewValue < MagTof_OldValue){ //The Zero value for Old value can be changed in the future to the value equal to the sensor home distant from the bottom of the slide
              RandomInt = PrevRandInt; //Find a new random number ^look up at random function^
              ProxCounter = 0; //Reset the count to zero 
                  if(MagTof_NewValue > MagTof_OldValue){ 
                      Reload=0; //Reset Main interrupt           
                      }
                      }
                      }
//End of Reload Program  
  }


    
//Random Function Called
if ((SwitchFunctionButton==3)&&(Reload!=1)){{

Blue_Led_1 = HIGH;
Blue_Led_2 = HIGH;
Blue_Led_3 = HIGH;

      
      //Generate a random number. If it's that same as the previous number, generate a new number
       RandomInt = random(16); //Random number between 0-15
          while(RandomInt == PrevRandInt){
          RandomInt = random(16); //Random number between 0-15
          }
  
    // Detect prox sensor going high and count number of occurences 
        if (ProxState = RISING){  
            ProxCounter++;
            }  

    //Stop main interrupt from functioning when the random number and counter number are the same
       if (ProxCounter == RandomInt){
            Malfunction = 1;    //In main interrupts Malfunction!=1
            }

    //If Momentary is pushed then Tof reaches certain distance go high then malfunction is reset to zero and then the cycle will continue to loop until 15 actuations are reached
       if (Malfunction = 1){
         //Have an interrupt to activate sensors
           digitalRead (MagButton); //"Mag" is tapped
            delay 500 //Time between tapping mag and pulling slide
                if(MagButton == HIGH){
                 analogRead (SlideTof);
                }
                }
                 if (SlideTof> XXX){  //Tof sensor says slide was pulled back X distance ie racked. Solenoid is turned off 
                   Malfunction = 0;     //Malfunction is cleared
                   }
                                 
    // interrupt main interrupt; //stops cycling.....Nest an if loop into interrupt function if("Reload!=1")
      if (ProxCounter == 15){
          Reload = 1;
          } 
          }
     if (Reload == 1){ //Tof Sensor on top of Mag compares distance value. When the distance increases (the mag has been dropped) the loop finds a new number, ProxCounter resets. When the distance decreases (the mag has been reinserted) the reload is reset to zero. 
          analogRead (MagTof_NewValue); //Read the input on analog pin
             if(MagTof_NewValue < MagTof_OldValue){ //The Zero value for Old value can be changed in the future to the value equal to the sensor home distant from the bottom of the slide
                  RandomInt = PrevRandInt; //Find a new random number ^look up at random function^
                  ProxCounter = 0; //Reset the count to zero 
                   Mag_Removed = 1; 
             }
                     if(Mag_Removed = 1){ 
                      analogRead (MagTof_NewValue); //Read the input on analog pin
                     }
                     }
                     if(MagTof_NewValue > MagTof_OldValue){
                        Reload=0; //Reset Main interrupt           
                         }
                         }
//End of Random Malfunction Program 
}

  
    
   
