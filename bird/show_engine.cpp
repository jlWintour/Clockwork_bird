//------------------------------------------------------------------------
// File: show_engine.cpp
//
// Description:
//   Executes a queue of motor moves,  
//

#include <Arduino.h>
#include <inttypes.h>
#include "Prop_motor.h"
#include "show_engine.h"
#include "bird.h"

Show_engine::Show_engine(void) {
  rd_ptr = 0;
  wr_ptr = 0;  
  for(int i=0; i<QUEUE_SIZE; i++) {
    queue[i] = NULL_MOVE;
  }

  timer = 1000;
}
 

//------------------------------------------- 
void Show_engine::tick(void) {
//------------------------------------------- 
  move_t move;
  int nq;

  if (timer) {
    timer--;
  }
  else {
    timer = 1000;  // default if show queue is empty
    nq = dequeue(&move);
    if ( nq == 0) return;

    // Execute dequeued move on props
    Serial.print("Engine[");
    Serial.print(nq);
    Serial.print("]: Prop:");
    Serial.print(props[move.prop]->get_servo());
    Serial.print(", target:");
    Serial.print(move.target);
    Serial.print(", prop_interval:");
    Serial.print(move.prop_interval);
    Serial.print(", gesture_interval:");
    Serial.println(move.gesture_interval);
    props[move.prop]->goto_target(move.target, move.prop_interval);
    timer = move.gesture_interval;
  } 
} 


//------------------------------------------- 
bool Show_engine::enqueue(move_t move) {
//------------------------------------------- 
  int space;

  space = (wr_ptr < rd_ptr) ? rd_ptr - wr_ptr : rd_ptr + QUEUE_SIZE - wr_ptr;
    
  if (space == 0) {
    Serial.println("Enqueue: no space");
    return false;
  }
  //Serial.print("Enqueue: Prop: ");
  //Serial.print(move.prop->get_servo());
  //Serial.print(", target:");
  //Serial.print(move.target);
  //Serial.print(", prop_interval: ");
  //Serial.print(move.prop_interval);
  //Serial.print(", gesture_interval:");
  //Serial.println(move.gesture_interval);

  queue[wr_ptr] = move;
  wr_ptr = (wr_ptr == QUEUE_SIZE-1) ? 0 : wr_ptr + 1; 
  return true;
} 
 

//------------------------------------------- 
int Show_engine::dequeue(move_t* p) {
//------------------------------------------- 
  int fill; move_t m;

  fill = (rd_ptr <= wr_ptr) ? wr_ptr - rd_ptr : wr_ptr + QUEUE_SIZE - rd_ptr;
    
  if (fill <= 0) return 0;

  // Serial.print("Dequeue: wp=");
  // Serial.print(wr_ptr);  
  // Serial.print(", rp=");  
  // Serial.print(rd_ptr);  
  // Serial.print(", f=");  
  // Serial.print(fill);  

  m = queue[rd_ptr];
  *p = m;

  rd_ptr = (rd_ptr == QUEUE_SIZE-1) ? 0 : rd_ptr + 1;
  return fill;
} 
