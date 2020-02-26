#include "avr/interrupt.h" 
#include "VL53L0X.h"
#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// C O N S T A N T S
const byte NUM_BULLETS = 17;

// G L O B A L S
volatile byte mode = 1;
volatile int barrell_bullet_count = 0;
volatile bool jammed = false;


continuos_mode_ops()
  // Motor turns on for short encoder steps/time
  // Motor drives in -X direction until it hits a limit switch


reload_mode_ops()
  if  count < barrell_bullet_count 
    // continuos_mode_ops()
  else 
    // motor turn long encoder steps
    // (slide locked open at this point)
    // motor return home
    // wait forever for a new magazine greeting interrupt



new magazine greeting ISR()
  // store magazine name and current_bullets_used in memory
  // send an acknowledge back to the mag to keep it from going to sleep
  // reset count to what the current_bullets_used from the mag
  // tap-rack-bang(2 seconds)
  // (dont talk to other mags until current mag empty/unresponsive)




(prox sensor trigger this ISR)
prox sensor ISR()
  set_mag_count(++barrell_bullet_count)
  switch (MODE)
    case 1:
      continuos_mode_ops()
      break;
    case 2:
      reload_mode_ops()
      break;
    case 3:
      rand_mode_ops()
      break;




mode button ISR()
  switch (++mode)
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



rand_mode_ops()
  int RandomInt = random(NUM_BULLETS + 1); //Random number between 0-NUM_BULLETS
  if (barrell_bullet_count != RandomInt){
    reload_mode_ops()
  }
  else {
    jammed = true;
    // wait forever for tap-rack-bang
  }




tap-rack-bang(wait_seconds)
  // wait for dist sensor to show slide has been fully extended (no more than wait_seconds)
  jammed = false;