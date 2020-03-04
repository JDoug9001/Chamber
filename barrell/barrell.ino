#include "avr/interrupt.h" 
#include "VL53L0X.h"
#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// C O N S T A N T S
const byte NUM_BULLETS = 17;
const int SHORT_NUM_STEPS = 1;
const int LONG_NUM_STEPS = 2;
const int TAP_RACK_BANG_TIME = 2; // seconds
const int TRB_WAIT_INTERVAL = 10; // miliseconds

// G L O B A L S
volatile byte mode = 1;
volatile int barrell_bullet_count = 0;
volatile bool jammed = false;
volatile bool new_magazine = false;


void continuos_mode_ops(){
  // Motor turns on for short encoder steps/time
  drive_motor(SHORT_NUM_STEPS);
  return_home();
}


void reload_mode_ops() {
  if (barrell_bullet_count < NUM_BULLETS){
    continuos_mode_ops();
  else 
    // motor turn long encoder steps
    drive_motor(LONG_NUM_STEPS);
    
    /* 
    // might need to wait until mag has extended the follower before return slide to home
    // need to experiment to find out for sure
    while (!mag_follower_extended()){
      delay(1);
    } 
    */

    // (slide locked open at this point -- this is done on the mag side)
    return_home();

    // wait forever for a new magazine greeting interrupt
    while (!new_magazine){
      delay(500);
    } 

    // made it past the wait for new mag. now, set new mag to false
    new_magazine = false;
  }
}


// this function happens everytime the mag button is pressed
void new_magazine_greeting_ISR(){
  // store magazine name and current_bullets_used in memory
  // send an acknowledge back to the mag to keep it from going to sleep
  // reset count to the current_bullets_used from the mag
  // tap_rack_bang()
  // (dont talk to other mags until current mag empty/unresponsive)

  // tell the code there is a new magazine
  new_magazine = true;
}


void drive_motor(int num_steps){
  // TODO: figure out what this looks like once we get an encoder
}


void return_home(){
  // over and over forever
  while (true) {
    // turn backward one step
    drive_motor(-1);
    // if it hits the limit switch
    if (limit_switch_pressed){
      // bail out of loop
      break;
    }
  }
}


// prox sensor trigger this ISR
void prox_sensor_ISR(){
  set_mag_count(++barrell_bullet_count);
  switch (mode){
    case 1:
      continuos_mode_ops();
      break;
    case 2:
      reload_mode_ops();
      break;
    case 3:
      rand_mode_ops();
      break;
  }
}


void mode_button_ISR(){
  switch (++mode){
    case 2:
      // light the 2nd LED
      Blue_Led_1 = LOW;
      Blue_Led_2 = HIGH; 
      Blue_Led_3 = LOW;
      break;
    case 3:
      // light the 3rd LED
      Blue_Led_1 = LOW;
      Blue_Led_2 = LOW; 
      Blue_Led_3 = HIGH;
      break;
    default:
      mode = 1;
      // light the 1st LED
      Blue_Led_1 = HIGH;
      Blue_Led_2 = LOW;
      Blue_Led_3 = LOW;
  }
}


void rand_mode_ops(){
  int RandomInt = random(NUM_BULLETS + 1); //Random number between 0-NUM_BULLETS
  if (barrell_bullet_count != RandomInt){
    reload_mode_ops();
  }
  else {
    jammed = true;

    // this looks really funny here but it is legit. when the mag button is pressed, 
    // the same message is sent to the chamber regardless of whether it was a 
    // 'tap-rack-bang' button press or a reload button press
    
    // wait forever for tap-rack-bang
    while (jammed){
      delay(10);
    } 
  }
}


void tap_rack_bang(){
  // wait 'wait_seconds' for dist sensor to show slide has been fully extended
  float max_i = TAP_RACK_BANG_TIME*1000 / TRB_WAIT_INTERVAL;
  for (i = 0; i < max_i; i++){
    delay(10);
    if (slide_fully_extended){
      jammed = false;
      break;
    }
  }
}


void set_mag_count(int num_rounds){
  //  TODO: figure out how to communicate to the mag and give it its num_rounds_fired count
}