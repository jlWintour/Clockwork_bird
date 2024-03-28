//-----------------------------------------------------------
// Sketch: Bird.ino
// Platform: Arduino Nano
//
// Description:
//
//
// #define DECODE_NEC  
// #define NO_LED_FEEDBACK_CODE     
// #define EXCLUDE_UNIVERSAL_PROTOCOLS  
// #define RAW_BUFFER_LENGTH 80
// #define IR_SEND_PIN 10
#include <Arduino.h>
#include "TinyIRReceiver.hpp"
#include <PCA9685.h>
#include <Button.h>
#include "Prop_motor.h"
#include "Show_engine.h"
#include "bird.h"


// Pin Definition
// #define       IR_INPUT_PIN      = 2;
const uint8_t MP3_BUSY_PIN      = 12;
const uint8_t TOP_MOTOR_M1P_PIN = 8;
const uint8_t TOP_MOTOR_M1N_PIN = 9;
const uint8_t TOP_MOTOR_M2P_PIN = 10;
const uint8_t TOP_MOTOR_M2N_PIN = 11;

// PWM Channel Assignment
const uint8_t PWM_OE_PIN        = A3;

const int LEFT_WING_PWM  = 0;
const int RIGHT_WING_PWM = 1;
const int NECK_PWM       = 2;
const int TOP_GEAR_PWM   = 3;
const int EYES_RD_PWM    = 4;
const int EYES_GN_PWM    = 5;
const int EYES_BL_PWM    = 6;

const int MP3_IO0_PWM    =  8;
const int MP3_IO1_PWM    =  9;
const int MP3_IO2_PWM    = 10;
const int MP3_IO3_PWM    = 11;
const int MP3_IO4_PWM    = 12;
const int MP3_IO5_PWM    = 13;
const int MP3_IO6_PWM    = 14;
const int MP3_IO7_PWM    = 15;

const int MP3_PWM[] = { MP3_IO0_PWM, MP3_IO1_PWM, MP3_IO2_PWM, MP3_IO3_PWM,
                        MP3_IO4_PWM, MP3_IO5_PWM, MP3_IO6_PWM, MP3_IO7_PWM
                      };  

/*
 * Helper macro for getting a macro definition as string
 */
#if !defined(STR_HELPER)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif

// IR Remote
const uint8_t MINUS_BUTTON = 0xFE;
const uint8_t PLUS_BUTTON  = 0xFC;
const uint8_t PLAY_BUTTON  = 0xFD;
const uint8_t MUTE_BUTTON  = 0xE6;
const uint8_t REW_BUTTON   = 0xED;
const uint8_t FFWD_BUTTON  = 0xFA;
const uint8_t UP_BUTTON    = 0xE1;
const uint8_t DOWN_BUTTON  = 0xF0;
const uint8_t MENU_BUTTON  = 0xE5;
const uint8_t ENTER_BUTTON = 0xF9;

volatile struct TinyIRReceiverCallbackDataStruct sCallbackData;

// Servo paramters
const uint16_t LOG_INTERVAL = 2000;

unsigned long last_millis;
uint16_t      log_timer;
uint8_t       show_state;

PCA9685 pwmController;     // Library using default B000000 (A5-A0) i2c address, and default Wire @400kHz

// Prop motor objects
Prop_motor left_wing( LEFT_WING_PWM,  LEFT_WING_MIN,  LEFT_WING_MAX,  LEFT_WING_DOWN );
Prop_motor right_wing(RIGHT_WING_PWM, RIGHT_WING_MIN, RIGHT_WING_MAX, RIGHT_WING_DOWN);
Prop_motor neck(      NECK_PWM,       NECK_LEFT,      NECK_RIGHT,     NECK_CENTER    );
Prop_motor red_eyes(  EYES_RD_PWM,    EYES_MIN,       EYES_MAX,       0              );
Prop_motor green_eyes(EYES_GN_PWM,    EYES_MIN,       EYES_MAX,       0x7ff          );
Prop_motor blue_eyes( EYES_BL_PWM,    EYES_MIN,       EYES_MAX,       0              );
Prop_motor top_gear(  TOP_GEAR_PWM,   CONTINUOUS_FWD, CONTINUOUS_REV);


const int N_PROPS = (sizeof(prop_id_t) + sizeof(int) /2) / sizeof(int) ;
            
// A table indexed by prop_id:
 const Prop_motor* props[] = { &left_wing, &right_wing, &neck, &top_gear,
                               &red_eyes,  &green_eyes, &blue_eyes
                             };

// Show machine
Show_engine show_engine;

bool        show_mode;
uint16_t    show_timer;
uint16_t    show_interval;
uint8_t     interval_select;
uint16_t    pos;
uint8_t     last_command;



//----------------------------------------------------------------
void setup() {
//----------------------------------------------------------------
  Serial.begin(115200);

  if (!initPCIInterruptForTinyReceiver()) {
    Serial.println(F("No interrupt available for pin " STR(IR_INPUT_PIN))); // optimized out by the compiler, if not required :-)
  }

  // Init the servos
  // Resets all PCA9685 devices on i2c line
  // Initializes module using default totem-pole driver mode, and default disabled phase balancer
  // Set PWM freq to 100Hz (default is 200Hz, supports 24Hz to 1526Hz)
  pinMode(PWM_OE_PIN, OUTPUT);
  digitalWrite(PWM_OE_PIN, HIGH);
  pinMode(2, INPUT);
  pinMode(MP3_BUSY_PIN, INPUT);

  delay(100);
  Wire.begin();
  pwmController.resetDevices();       
  pwmController.init();               
  pwmController.setPWMFrequency(100);
  pwmController.setChannelPWM(LEFT_WING_PWM, LEFT_WING_DOWN); 
  pwmController.setChannelPWM(RIGHT_WING_PWM, RIGHT_WING_DOWN); 
  pwmController.setChannelPWM(NECK_PWM, NECK_CENTER);
  pwmController.setChannelPWM(EYES_RD_PWM, 0x0);
  pwmController.setChannelPWM(EYES_GN_PWM, 0x0);
  pwmController.setChannelPWM(EYES_BL_PWM, 0x0);

  // Shows     
  show_timer     = 1000;
  show_interval  = 2000;
  show_mode      = false;
  last_command   = 0;
  mp3_timer      = 0;
  bird_shows_init();

  // Ready
  randomSeed(analogRead(A0));

  Serial.println("Hi!");
  // Enable Servos
  delay(2000);
  digitalWrite(PWM_OE_PIN, LOW);
  Serial.print("Enabled [");
  Serial.print(random(50));
  Serial.println("]");
}


//----------------------------------------------------------------
void loop() {
//----------------------------------------------------------------
  unsigned long this_millis;
  uint8_t  temp;

  // millisecond tick items
  this_millis = millis();
  if (this_millis != last_millis) {

    //--------[ Prop motors ]--------
    left_wing.tick();
    right_wing.tick();
    neck.tick();
    red_eyes.tick();
    green_eyes.tick();
    blue_eyes.tick();
    top_gear.tick();
    mp3_tick();

    //--------[ IR Remote Control ]--------
    if (sCallbackData.justWritten) {
      temp = sCallbackData.Flags;
      sCallbackData.justWritten = false;
      //if (temp != 0) {
      //  Serial.print("Flags = ");
      //  Serial.print(temp);
      //  Serial.print(" / ");
      //  Serial.println(temp);
      //}
      temp = temp & IRDATA_FLAGS_IS_REPEAT;
      if (temp == 0) {
        //Serial.print(F(" Command=0x"));
        //Serial.println(sCallbackData.Command, HEX);
        switch(sCallbackData.Command) {
          case MINUS_BUTTON: mp3_play_song(JAY_SONG);     break;
          case PLUS_BUTTON:  speed_up();                  break;
          case PLAY_BUTTON:  run_stop();                  break;
          case MUTE_BUTTON:  quiet();                     break;
          case REW_BUTTON:   look_left();                 break;
          case FFWD_BUTTON:  look_right();                break;
          case UP_BUTTON:    mp3_play_song(FINCH_SONG);   break;
          case DOWN_BUTTON:  mp3_play_song(REDWING_SONG); break; 
          case MENU_BUTTON:  startle();                   break;
          case ENTER_BUTTON: top_gear_rs();               break;
        }
        last_command = sCallbackData.Command;
      }
    }
    else {
      last_command = 0;
    }

    //--------[ The Show ]--------
    show_engine.tick();
    if (show_mode) long_run(); 

    last_millis = this_millis;
  }
}


//-----------------------------------------------------------------------------------
void handleReceivedTinyIRData(uint8_t aAddress, uint8_t aCommand, uint8_t aFlags) {
//-----------------------------------------------------------------------------------
  sCallbackData.Address = aAddress;
  sCallbackData.Command = aCommand;
  sCallbackData.Flags = aFlags;
  sCallbackData.justWritten = true;
}

