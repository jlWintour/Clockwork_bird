//----------------------------------------------------------------------
//
//  File: Prop_motor.h
//
// Description: 
//   Interface defs for the prop_motor object, a non-blocking way to script 
//   servo motor motion.
//
#ifndef Prop_motor_h
#define Prop_motor_h

#include <inttypes.h>

typedef enum {
  NO_TICK  = 1,
  FWD_TICK = 2,
  REV_TICK = 3,
  ALT_TICK = 4     
} tick_mode_t;

//---------------------------------------------------------------
class Prop_motor { 
//---------------------------------------------------------------
  public:
    Prop_motor(int srv_ch, int ba, int li, int init);
    Prop_motor(int srv_ch, int rev, int fwd);
    void     tick(void);
    void     goto_target(uint16_t tgt, uint16_t interval); 
    uint16_t get_posn(void);
    uint16_t get_target(void);
    bool     on_target(void);
    int      get_servo(void);
    void     set_tick_mode(tick_mode_t tm, uint16_t inter, uint16_t tcount);
    tick_mode_t get_tick_mode(void);

  private:
    tick_mode_t tick_mode;
    int         tick_state;
    uint16_t    tick_count;
    int         fwd_pwm_value;
    int         rev_pwm_value;
    uint16_t    timer;
    uint16_t    tick_interval;
    uint16_t    current_posn;
    uint16_t    base;
    uint16_t    limit;
    uint16_t    timer_reload;
    uint16_t    target;
    int         incr;
    int         motor;
    bool        continuous;
};

#endif
