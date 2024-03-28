//----------------------------------------------------------------------
//
//  File: bird.h
//
// Description: 
//   Shared defs for the bird machine.
//
#ifndef bird_h
#define bird_h

#include <inttypes.h>
#include <PCA9685.h>


extern const uint8_t MP3_BUSY_PIN;

// PWM Channel Assignment
extern const uint8_t PWM_OE_PIN;

extern const int LEFT_WING_PWM;
extern const int RIGHT_WING_PWM;
extern const int NECK_PWM;
extern const int TOP_GEAR_PWM;
extern const int EYES_RD_PWM;
extern const int EYES_GN_PWM;
extern const int EYES_BL_PWM;

extern const int MP3_PWM[];  

extern const Prop_motor* props[];

// Prop_ids are shorthand to identify props
typedef enum { LEFT_WING  = 0,
               RIGHT_WING = 1,
               NECK       = 2,
               TOP_GEAR   = 3,
               RED_EYES   = 4,
               GREEN_EYES = 5,
               BLUE_EYES  = 6
             } prop_id_t;

extern uint16_t show_timer;
extern uint16_t mp3_timer;
extern bool     show_mode;
extern uint16_t show_interval;
extern uint8_t  interval_select;
extern PCA9685  pwmController;    

extern Prop_motor left_wing;
extern Prop_motor right_wing;
extern Prop_motor neck;
extern Prop_motor red_eyes;
extern Prop_motor green_eyes;
extern Prop_motor blue_eyes;
extern Prop_motor top_gear;
extern Show_engine show_engine;

const int     CONTINUOUS_SPEED =  1;
const int     CONTINUOUS_FWD   = 425;
const int     CONTINUOUS_REV   = 800;
const int     CONTINUOUS_STOP  = 0;

const int     LEFT_WING_MIN   = 345; 
const int     LEFT_WING_MAX   = 955;  
const int     LEFT_WING_BASE  = 345;
const int     LEFT_WING_LIMIT = 700; 
const int     LEFT_WING_DOWN  = LEFT_WING_BASE;
const int     LEFT_WING_UP    = LEFT_WING_LIMIT;

const int     RIGHT_WING_MIN  =  345;
const int     RIGHT_WING_MAX   = 955;
const int     RIGHT_WING_BASE  = 485;
const int     RIGHT_WING_LIMIT = 870;
const int     RIGHT_WING_DOWN  = RIGHT_WING_LIMIT;
const int     RIGHT_WING_UP    = RIGHT_WING_BASE;

const int     NECK_MIN    = 390;  //  345;
const int     NECK_MAX    = 940;
const int     NECK_LEFT   = NECK_MAX;
const int     NECK_RIGHT  = NECK_MIN;
const int     NECK_MID_RIGHT  = 475;
const int     NECK_CENTER = 590; // (NECK_MIN + NECK_MAX + 1)/2;
const int     EYES_MIN    = 0;
const int     EYES_MID    = 0x7ff;
const int     EYES_MAX    = 0xfff;
extern const int CONTINUOUS_SPEED;
extern const int CONTINUOUS_FWD;
extern const int CONTINUOUS_BACK;
extern const int CONTINUOUS_STOP;

const uint8_t  MUTE_SONG     = 1; 
const uint8_t  RUN_SONG      = 2; 
const uint8_t  STOP_SONG     = 3; 
const uint8_t  SPEEDUP_SONG  = 4;
const uint8_t  SLOWDOWN_SONG = 5;
const uint8_t  FINCH_SONG    = 6;
const uint8_t  JAY_SONG      = 7;
const uint8_t  REDWING_SONG  = 8;
const uint8_t  CROW_SONG     = 9;
const uint8_t  SPEED0_SONG  = 10;
const uint8_t  SPEED1_SONG  = 11;
const uint8_t  SPEED2_SONG  = 12;
const uint8_t  SPEED3_SONG  = 13;

const uint8_t MIN_SHOW_SONG =  6;
const uint8_t MAX_SHOW_SONG =  9;

void mp3_tick(void);
void bird_shows_init(void);
void quiet(void);
void head_sweep(void);
void wing_ruffle(void);
void eye_colour(void);
void run_stop(void);
void long_run(void);
void mp3_play_song(uint8_t song);
void startle(void);
void left_wing_rise(void);
void right_wing_rise(void);
void look_left(void);
void look_right(void);
void top_gear_rs(void);
void speed_up(void);
void slow_down(void);
#endif
