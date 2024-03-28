//------------------------------------------------------------------------
// File:bird_shows.cpp
//
// Description:
//   A collection of simple motion routines, each of which queues a sequence of moves
//   on the show engine.
//
#include <Arduino.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "Prop_motor.h"
#include "Show_engine.h"
#include "bird.h"

typedef enum { EYES_BLACK = 0,
               EYES_RED   = 1,
               EYES_GREEN = 2,
               EYES_BLUE  = 3
             } eye_state_t;

const uint8_t N_EYE_STATES = 4;
eye_state_t eye_state;


uint8_t     state;
uint16_t    timer;

typedef enum { EYES_GESTURE     = 0,
               WING_GESTURE     = 1,
               NECK_GESTURE     = 2,
               TOP_GEAR_GESTURE = 3,
               SOUND_GESTURE    = 4
             } gesture_type_t;

gesture_type_t gesture_type;
const char N_GESTURE_TYPES = 5;

// Show Intervals
const int N_SPEEDS = 4;
const uint16_t SHOW_INTERVALS[] = { 30000, 10000, 5000, 1500};
const uint8_t  INTERVAL_SONGS[] = {SPEED0_SONG, SPEED1_SONG, SPEED2_SONG, SPEED3_SONG};

// --------[ Eye Colour ]--------
const PROGMEM move_t eyes_black_seq[] = {
    (move_t){prop: RED_EYES,   target: EYES_MIN, prop_interval : 1, gesture_interval : 1},
    (move_t){prop: GREEN_EYES, target: EYES_MIN, prop_interval : 1, gesture_interval : 1},
    (move_t){prop: BLUE_EYES,  target: EYES_MIN, prop_interval : 1, gesture_interval : 1}
};

const PROGMEM move_t eyes_red_seq[] = {
    (move_t){prop: RED_EYES,   target: EYES_MAX, prop_interval : 1, gesture_interval : 1},
    (move_t){prop: GREEN_EYES, target: EYES_MIN, prop_interval : 1, gesture_interval : 1},
    (move_t){prop: BLUE_EYES,  target: EYES_MIN, prop_interval : 1, gesture_interval : 1}
};

const PROGMEM move_t eyes_green_seq[] = {
    (move_t){prop: RED_EYES,   target: EYES_MIN, prop_interval : 1, gesture_interval : 1},
    (move_t){prop: GREEN_EYES, target: EYES_MAX, prop_interval : 1, gesture_interval : 1},
    (move_t){prop: BLUE_EYES,  target: EYES_MIN, prop_interval : 1, gesture_interval : 1}
};

const PROGMEM move_t eyes_blue_seq[] = {
    (move_t){prop: RED_EYES,   target: EYES_MIN, prop_interval : 1, gesture_interval : 1},
    (move_t){prop: GREEN_EYES, target: EYES_MIN, prop_interval : 1, gesture_interval : 1},
    (move_t){prop: BLUE_EYES,  target: EYES_MAX, prop_interval : 1, gesture_interval : 1}
};

const PROGMEM move_t eyes_temp_blue_seq[] = {
    (move_t){prop: RED_EYES,   target: EYES_MIN, prop_interval:   1, gesture_interval :    1},
    (move_t){prop: GREEN_EYES, target: EYES_MIN, prop_interval:   1, gesture_interval :    1},
    (move_t){prop: BLUE_EYES, target: EYES_MAX, prop_interval:    1, gesture_interval : 2500},
    (move_t){prop: BLUE_EYES, target: EYES_MIN, prop_interval: 1000, gesture_interval :  100}
};

// --------[ Head Sweep ]--------
const PROGMEM move_t head_sweep_seq[] = {
  (move_t){prop: RED_EYES,   target: EYES_MIN,    prop_interval:    1, gesture_interval:    1},
  (move_t){prop: GREEN_EYES, target: EYES_MAX,    prop_interval:    1, gesture_interval:    1},
  (move_t){prop: BLUE_EYES,  target: EYES_MIN,    prop_interval: 1000, gesture_interval:    1},
  (move_t){prop: NECK,       target: NECK_LEFT,   prop_interval: 1500, gesture_interval: 1500},
  (move_t){prop: NECK,       target: NECK_RIGHT,  prop_interval: 3000, gesture_interval: 3000},
  (move_t){prop: NECK,       target: NECK_CENTER, prop_interval: 1500, gesture_interval: 1500},
  (move_t){prop: GREEN_EYES, target: EYES_MIN,    prop_interval: 1000, gesture_interval: 1000}
};

//--------[ Left Wing Rise ]--------]
const PROGMEM move_t leftwing_rise_seq[] = {
  (move_t){prop : LEFT_WING, target : LEFT_WING_UP,   prop_interval : 250, gesture_interval :  500},
  (move_t){prop : LEFT_WING, target : LEFT_WING_DOWN, prop_interval : 2000, gesture_interval :2000}
};

//--------[ Right Wing Rise ]--------]
const PROGMEM move_t rightwing_rise_seq[] = {
  (move_t){prop : RIGHT_WING, target : RIGHT_WING_UP,   prop_interval :  250, gesture_interval:  500},
  (move_t){prop : RIGHT_WING, target : RIGHT_WING_DOWN, prop_interval : 2000, gesture_interval: 2000}
};

//--------[ Wing Ruffle ]--------]
const PROGMEM move_t wing_ruffle_seq[] = {
  (move_t){prop: LEFT_WING,  target: LEFT_WING_UP,    prop_interval: 250, gesture_interval: 250},
  (move_t){prop: LEFT_WING,  target: LEFT_WING_DOWN,  prop_interval: 250, gesture_interval:   1},
  (move_t){prop: RIGHT_WING, target: RIGHT_WING_UP,   prop_interval: 250, gesture_interval: 250},
  (move_t){prop: RIGHT_WING, target: RIGHT_WING_DOWN, prop_interval: 250, gesture_interval:   1},
  (move_t){prop: LEFT_WING,  target: LEFT_WING_UP,    prop_interval: 250, gesture_interval: 250},
  (move_t){prop: LEFT_WING,  target: LEFT_WING_DOWN,  prop_interval: 250, gesture_interval:   1},
  (move_t){prop: RIGHT_WING, target: RIGHT_WING_UP,   prop_interval: 250, gesture_interval: 250},
  (move_t){prop: RIGHT_WING, target: RIGHT_WING_DOWN, prop_interval: 250, gesture_interval: 250}
};

//--------[ Startle ]--------]
const PROGMEM move_t startle_seq[] = {
  (move_t){prop:RED_EYES,    target:EYES_MAX,        prop_interval:   1, gesture_interval:    1},
  (move_t){prop: GREEN_EYES, target : EYES_MIN,      prop_interval:   1, gesture_interval:    1},
  (move_t){prop: BLUE_EYES,  target : EYES_MIN,      prop_interval:   1, gesture_interval:    1},
  (move_t){prop:LEFT_WING,   target:LEFT_WING_UP,    prop_interval: 200, gesture_interval:    1},
  (move_t){prop:RIGHT_WING,  target:RIGHT_WING_UP,   prop_interval: 200, gesture_interval: 2000},
  (move_t){prop:LEFT_WING,   target:LEFT_WING_DOWN,  prop_interval: 2000, gesture_interval:  100},
  (move_t){prop:RIGHT_WING,  target:RIGHT_WING_DOWN, prop_interval: 2000, gesture_interval:    1}
};

//--------[ Look Left ]--------]
const PROGMEM move_t look_left_seq[] = {
  (move_t){prop: RED_EYES,   target: EYES_MIN,    prop_interval:    1, gesture_interval:    1},
  (move_t){prop: GREEN_EYES, target: EYES_MAX,    prop_interval:    1, gesture_interval:    1},
  (move_t){prop: BLUE_EYES,  target: EYES_MIN,    prop_interval: 1000, gesture_interval:    1},
  (move_t){prop: NECK,       target:NECK_LEFT,    prop_interval:  200, gesture_interval: 2000},
  (move_t){prop: NECK,       target: NECK_CENTER, prop_interval: 2000, gesture_interval:    1},
  (move_t){prop: GREEN_EYES, target: EYES_MIN,    prop_interval: 1000, gesture_interval: 1000}
};

//--------[ Look Right ]--------]
const PROGMEM move_t look_right_seq[] = {
  (move_t){prop: RED_EYES,   target: EYES_MIN,    prop_interval:    1, gesture_interval:    1},
  (move_t){prop: GREEN_EYES, target: EYES_MAX,    prop_interval:    1, gesture_interval:    1},
  (move_t){prop: BLUE_EYES,  target: EYES_MIN,    prop_interval: 1000, gesture_interval:    1},
  (move_t){prop: NECK,       target: NECK_RIGHT,  prop_interval:  200, gesture_interval: 2000},
  (move_t){prop: NECK,       target: NECK_CENTER, prop_interval: 2000, gesture_interval:    1},
  (move_t){prop: GREEN_EYES, target: EYES_MIN,    prop_interval: 1000, gesture_interval: 1000}
};

//--------[ Gesture Libarary ]--------
typedef struct {
  move_t *seq;
  int entries;
} gesture_t;


//---------------------------------------------------------------
//            Shows Init
//---------------------------------------------------------------
void bird_shows_init()
{
  top_gear.set_tick_mode(NO_TICK, 0, 0);
  state        = 0; 
  gesture_type = SOUND_GESTURE;
  eye_state    = EYES_BLUE;
  timer        = 1000;
  show_interval   = 1000; // 30000;
  interval_select = 0;
  show_interval = SHOW_INTERVALS[interval_select];
}

//--------------------------------------------------------------------
//                       Quiet 
//--------------------------------------------------------------------
void quiet(void) {
  left_wing.goto_target(LEFT_WING_DOWN, 100);
  right_wing.goto_target(RIGHT_WING_DOWN, 100);
  neck.goto_target(NECK_CENTER, 100);
  red_eyes.goto_target(EYES_MIN, 100);
  green_eyes.goto_target(EYES_MIN, 100);
  blue_eyes.goto_target(EYES_MID, 100);
  top_gear.set_tick_mode(NO_TICK, 100, 100);
  mp3_play_song(MUTE_SONG);
  Serial.println(F("Quiet"));
}

//---------------------------------------------------------------
void run_stop() {
//---------------------------------------------------------------
  show_mode = !show_mode;
  if (show_mode) {
    Serial.println(F("Run"));
    mp3_play_song(RUN_SONG);
  }
  else {
    Serial.println(F("Stop"));
    mp3_play_song(STOP_SONG);
  }
}

//---------------------------------------------------------------
void speed_up() {
//---------------------------------------------------------------
  interval_select = (interval_select==3) ? 0 : interval_select+1;
  show_interval = SHOW_INTERVALS[interval_select];
  mp3_play_song(INTERVAL_SONGS[interval_select]);
  Serial.print(F("Speed up: "));
  Serial.print(interval_select);
  Serial.print(": ");
  Serial.print(show_interval);
  Serial.print(", ");
  Serial.println(INTERVAL_SONGS[interval_select]);
}

//---------------------------------------------------------------
void slow_down() {
//---------------------------------------------------------------
  show_interval = (show_interval < 30000) ? show_interval + 1000 : 20000;
  mp3_play_song(SLOWDOWN_SONG);
  Serial.print(F("Slow down: "));
  Serial.println(show_interval);
}

//---------------------------------------------------------------
void enq_sequence(move_t* seq, int entries) {
//---------------------------------------------------------------
  move_t move;
  for (int i = 0; i < entries; i++) {
    memcpy_P(&move, &seq[i], sizeof(move_t));
    if (!show_engine.enqueue(move)) {
      Serial.println(F("*Overflowed*"));
    }
  }
}

//---------------------------------------------------------------
void long_run() {
//---------------------------------------------------------------
//  Pick a random gesture class, and then a random gesture within
//  the class.
  move_t *seq;
  move_t move;
  int    entries;
  int    selection;
  static uint8_t last_sound;
  static uint8_t last_neck;
  static uint8_t last_wing;

  if (timer) {
    timer--;
  }
  else {
    // Pick a next gesture_type, one that's not the same as the last one
    selection = gesture_type;
    while (selection == gesture_type) selection = random(0, N_GESTURE_TYPES);
    gesture_type = selection;
    Serial.print(F("Gesture type "));
    Serial.println(gesture_type);

    switch (gesture_type) {
      case EYES_GESTURE:
        // Pick a new eye_state, one that's not the same as the last one
        selection  = eye_state;
        while (selection == eye_state) selection = random(0, N_EYE_STATES);
        eye_state = selection;
        Serial.print(F("eye_state = "));
        Serial.println(eye_state);
        entries   = sizeof(eyes_black_seq)/sizeof(move_t);
        switch(eye_state) {
          case EYES_BLACK: enq_sequence(eyes_black_seq, entries); break;
          case EYES_RED:   enq_sequence(eyes_red_seq,   entries); break;
          case EYES_GREEN: enq_sequence(eyes_green_seq, entries); break;
          case EYES_BLUE:  enq_sequence(eyes_blue_seq,  entries); break;
        }
        timer = 5;
        break;

      case WING_GESTURE:    
        Serial.print(F("wing "));
        Serial.println(last_wing);
        selection  = last_wing;
        while (selection == last_wing) selection = random(0, 3);
        last_wing = selection;
        switch(last_wing) {
          case 0: left_wing_rise();  break; 
          case 1: right_wing_rise(); break;
          case 2: wing_ruffle();     break;
        }
        timer = 3000;
        break;

      case NECK_GESTURE:  
        // 3 neck gestures: look_left, look_right, head_sweep
        selection  = last_neck;
        while (selection == last_neck) selection = random(0, 3);
        last_neck = selection;
        Serial.print(F("neck "));
        Serial.println(last_neck);
        switch(last_neck) {
          case 0: look_left();  break; 
          case 1: look_right(); break;
          case 2: head_sweep(); break;
        }
        timer = 3000;
        break;

      case TOP_GEAR_GESTURE:
        top_gear.set_tick_mode(FWD_TICK, 150, random(3, 17));
        timer = 2000;
        break;

      case SOUND_GESTURE:
        // Pick a new sound, not the same as the last one
        selection = last_sound;
        while(selection == last_sound) selection = random(MIN_SHOW_SONG, MAX_SHOW_SONG+1);
        last_sound = selection;
        Serial.print(F("new_song = "));
        Serial.println(last_sound);
        entries   = sizeof(eyes_temp_blue_seq)/sizeof(move_t);
        enq_sequence(eyes_temp_blue_seq, entries);
        mp3_play_song(last_sound); 
        timer = 3000;
        break;
    }
    //  Timer delay is the show interval randomized by up to 50%
    timer += show_interval/2 + random(0, show_interval);
    Serial.print("timer = ");
    Serial.println(timer);
  }
}

//---------------------------------------------------------------
//            Head Sweep
//---------------------------------------------------------------
void head_sweep() {
  int entries = sizeof(head_sweep_seq) / sizeof(move_t);
  Serial.println("Head Sweep");
  enq_sequence(head_sweep_seq, entries);
  timer = 5000;
}

//---------------------------------------------------------------
//            Left Wing Rise
//---------------------------------------------------------------
void left_wing_rise() {
  int entries = sizeof(leftwing_rise_seq) / sizeof(move_t);
  Serial.println("Wing Ruffle");
  enq_sequence(leftwing_rise_seq, entries);
  timer = 5000;
}

//---------------------------------------------------------------
//            Right Wing Rise
//---------------------------------------------------------------
void right_wing_rise() {
  int entries = sizeof(rightwing_rise_seq) / sizeof(move_t);
  Serial.println("Wing Ruffle");
  enq_sequence(rightwing_rise_seq, entries);
  timer = 5000;
}

//---------------------------------------------------------------
//            Startle
//---------------------------------------------------------------
void startle() {
  int entries = sizeof(startle_seq) / sizeof(move_t);
  Serial.println("Startle");
  mp3_play_song(CROW_SONG);
  enq_sequence(startle_seq, entries);
  timer = 3000;
}

//---------------------------------------------------------------
//            Wing Ruffle
//---------------------------------------------------------------
void wing_ruffle() {
  int entries = sizeof(wing_ruffle_seq) / sizeof(move_t);
  Serial.println("Wing Ruffle");
  enq_sequence(wing_ruffle_seq, entries);
  timer = 5000;
}

//---------------------------------------------------------------
//            Look Left
//---------------------------------------------------------------
void look_left()
{
  int entries = sizeof(look_left_seq) / sizeof(move_t);
  Serial.println(F("Look Left"));
  enq_sequence(look_left_seq, entries);
  timer = 3000;
}

//---------------------------------------------------------------
//            Look Right
//---------------------------------------------------------------
void look_right()
{
  int entries = sizeof(look_right_seq) / sizeof(move_t);
  Serial.print(F("Look Right ["));
  enq_sequence(look_right_seq, entries);
  timer = 3000;
}

//---------------------------------------------------------------
//            Top Gear Run/Stop
//---------------------------------------------------------------
void top_gear_rs()
{
  if (top_gear.get_tick_mode() == NO_TICK) {
    Serial.println(F("Tick Run"));
    top_gear.set_tick_mode(FWD_TICK, 150, 0);
  }
  else {
    Serial.println(F("Tick Stop"));
    top_gear.set_tick_mode(NO_TICK, 150, 0);
  }
}
