//------------------------------------------------------------------------
// File:bird_songs.cpp
//
// Description:
//   A collection of simple motion routines, each of which queues a sequence of moves
//   on the show engine.
//
#include <Arduino.h>
#include <inttypes.h>
#include "Prop_motor.h"
#include "Show_engine.h"
#include "bird.h"

uint16_t mp3_timer;

void mp3_tick() {
  if (mp3_timer == 1) {
//    Serial.print(F("Done. Busy = "));
//    Serial.println(digitalRead(MP3_BUSY_PIN));
    for (int i=0; i<8; i++) {
      pwmController.setChannelPWM(MP3_PWM[i], 0x1fff);
    }
  }
  if (mp3_timer > 0) {
    mp3_timer--;
  }
}

void mp3_play_song(uint8_t song) {
   uint16_t v;

//   Serial.print(F("Play Song "));
//   Serial.print(song);
//   Serial.print(F(" Busy = "));
//   Serial.println(digitalRead(MP3_BUSY_PIN));
   
   for (int i=0; i<8; i++) {
      v = (song >> i) & 1;
      v = (v == 0) ? 0x1fff : 0;
      pwmController.setChannelPWM(MP3_PWM[i], v); 
      mp3_timer = 100;
   }
//   Serial.print("\n");
}