//-------------------------------------------------------
//  File: Prop_motor.cpp
//
//
#include <Arduino.h>
#include <inttypes.h>
#include <PCA9685.h>
#include "Prop_motor.h"

extern PCA9685 pwmController;

const uint16_t IDLE_TIME = 1000;


//--------------------------------------------------------------------------------------
Prop_motor::Prop_motor(int srv_ch, int ba, int li, int init) { // for positional motors
//--------------------------------------------------------------------------------------
  tick_mode     = NO_TICK;
  tick_state    = 0;
  current_posn  = init;
  base          = ba;
  limit         = li;
  fwd_pwm_value = 0;
  rev_pwm_value = 0;
  incr          = 1;
  timer_reload  = 100;
  tick_interval = IDLE_TIME;
  tick_count    = 0;
  target        = init;
  timer         = 0;
  motor         = srv_ch; 
  continuous    = false;    
}

//----------------------------------------------------------------------------------------------
Prop_motor::Prop_motor(int srv_ch, int fwd, int rev) { // For continuous rotation motors
//----------------------------------------------------------------------------------------------
  tick_mode     = NO_TICK;
  tick_state    = 0;
  current_posn  = 0;
  base          = 0;
  limit         = 0;
  fwd_pwm_value = fwd;
  rev_pwm_value = rev;
  incr          = 1;
  timer_reload  = 1000;
  target        = 0;
  timer         = 0;
  motor         = srv_ch;
  tick_interval = IDLE_TIME;
  tick_count    = 0;
  continuous    = true; 
}

//--------------------------
void Prop_motor::tick() {
//--------------------------
  int distance;

  if (timer > 0) {
    timer--;
  } 
  else {
    // Time for a position update.
    if(continuous) {
      // Continuous rotation motor
      switch (tick_mode) {
        case FWD_TICK: 
          current_posn = (tick_state == 0) ? fwd_pwm_value : 0;  
          tick_state   = (tick_state < 1) ? tick_state + 1 : 0;
          break;

        case REV_TICK: 
          current_posn =  (tick_state == 0) ? rev_pwm_value : 0;  
          tick_state = (tick_state < 1) ? tick_state + 1 : 0;
          break;

        case ALT_TICK:  
          current_posn =  (tick_state == 0) ? fwd_pwm_value  
                        : (tick_state == 2) ? rev_pwm_value
                        : 0;
          tick_state = (tick_state < 3) ? tick_state + 1 : 0;
          break; 

        case NO_TICK:
          current_posn = 0;  
          break;

        default:        
          // NO_TICK: just stop motor
          Serial.println("Tick mode?");
          current_posn = 0;  
      }
      // See if it's time to stop ticking
      if (tick_count == 1) {
        //Serial.println(F("Stop_tick"));
        tick_mode  = NO_TICK;
        tick_state = 0;
      }
      tick_count =  (tick_count == 0) ? 0
                  : (tick_state == 0) ? tick_count - 1
                  : tick_count;

      timer_reload = tick_interval;
    }
    else {  
      // True servo - move the position toward the target                        
      if (current_posn == target) {
        timer_reload = IDLE_TIME;
        return;
      }

      distance = (target > current_posn) ? target - current_posn : current_posn - target;
  
      if (distance < abs(incr) ) {
        timer_reload = IDLE_TIME;
        current_posn = target;
      }
      else {
        //  Serial.print("Motor[");
        //  Serial.print(motor);
        //  Serial.print("]: cur:");
        //  Serial.print(current_posn);
        //  Serial.print(", tgt:");
        //  Serial.print(target);
        //  Serial.print(", incr:");
        //  Serial.println(incr);
        current_posn += incr;
        if (current_posn != target) timer = timer_reload;
      }
    }
    pwmController.setChannelPWM(motor, current_posn); 
    timer = timer_reload;
  }
}

//--------------------------------------------------------------
void Prop_motor::goto_target(uint16_t tgt, uint16_t interval) {
//--------------------------------------------------------------
  int distance;

  distance = (tgt > current_posn) ? tgt - current_posn : current_posn - tgt;
  distance = (distance == 0) ? 1 : distance;

  // Set increment so that we move in hops of at least 3 ms
  incr = 1;
  while (interval * incr / distance < 2) incr *= 2;

  timer_reload = (interval * incr / distance);
  //Serial.print(F(", incr:"));
  //Serial.print(incr);
  //Serial.print(F(", int:"));
  //Serial.print(interval);
 //Serial.print(F(", Reload:"));
  //Serial.println(timer_reload);

  timer_reload = (timer_reload == 0) ? 1 : timer_reload; 
  incr = (tgt < current_posn) ? -incr : incr;


  //Serial.print(F("tgt:"));
  //Serial.print(tgt);
  //Serial.print(F(", int:"));
  //Serial.print(interval);
  //Serial.print(F(", dst:"));
  //Serial.print(distance);
  //Serial.print(F(", incr:"));
  //Serial.print(incr);
  //Serial.print(F(", rel:"));
  //Serial.println(timer_reload);

  target = tgt;
  timer = timer_reload;
}

uint16_t Prop_motor::get_posn() {
  return current_posn;
}

//------------------------------------------------------------------------------------
void Prop_motor::set_tick_mode(tick_mode_t tm, uint16_t inter, uint16_t tcount) {
//------------------------------------------------------------------------------------
  // tick_count of 0 means run forever
  timer_reload =  (tm == NO_TICK) ? IDLE_TIME
                : (inter == 0)    ? IDLE_TIME 
                : inter;
  tick_mode  = tm;
  tick_interval = inter;
  tick_count = tcount;
  tick_state = 0;
  Serial.print("Tick mode ");
  Serial.println(tick_mode);
}


//--------------------------------------------------------------------
tick_mode_t Prop_motor::get_tick_mode(void) {
//--------------------------------------------------------------------
  return tick_mode;
}


uint16_t Prop_motor::get_target() {
  return target;
}
bool Prop_motor::on_target() {
  return (current_posn == target);
}
int Prop_motor::get_servo() {
  return motor;
}

