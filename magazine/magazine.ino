


// mag button press interrupts sleep
mag_button_interrupt()
     if hall effect sensor is high
          send device name and current_bullets_used to barrell
          wait_for_acknowledge(ACK_TIME)
          if not acknowledged
                go_to_sleep()





go_to_sleep()
     sleep



// communication interrupt - either set_mag_count or an ack
if the incoming transmission is setting the number of  bullets
     current_bullets_used = barrell_bullet_count
else
     acknowledged = true



// long press of mag button causes bullet count to reset to 0.