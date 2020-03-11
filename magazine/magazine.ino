#include <Wire.h>


// define variables
volatile int number_shots_fired;
const int ACK_TIME = 1000;

// mag button press interrupts sleep
void mag_button_interrupt(){
     // if the hall effect sensor is close to metal
     if (hall_effect_sensor.read()){
          // send device name and current_bullets_used to barrell
          send_greeting();
          if (!is_acknowledged(ACK_TIME)){
               go_to_sleep();
          }
     }
}



bool is_acknowledged(int wait_ms){
     bool ack = false;
     int elapsed_t = 0;
     // while not acknowledged and time not expired
     while (!ack & wait_ms < elapsed_t){
          // wait a ms
          delay(1);
          ++elapsed_t;
     }
     return  ack;        
}



void send_greeting(){
     char name[] = "0123456789ABCDEF";
     // read stored variable shots_fired
     number_shots_fired = read_shots_from_eprom();
     tx_data_string(name);
     tx_data_int(number_shots_fired);
}


int read_shots_from_eprom(){
     return 0; //TODO: fix this function to actually work
}


go_to_sleep(){
     // TODO: put microncontroler to sleep
}



void received_message_ISR(){
     //figure this part out
}





void tx_data_string(name){
     // for loop to send a char[] (a bunch of chars)
}



void tx_data_int(number_shots_fired){
     // send an int
}





// communication interrupt - either set_mag_count or an ack
if the incoming transmission is setting the number of  bullets
     current_bullets_used = barrell_bullet_count
else
     ack = true;



//TODO: long press of mag button causes bullet count to reset to 0.