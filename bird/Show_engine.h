//----------------------------------------------------------------------
//
//  File: Show_engine.h
//
// Description: 
//   Interface defs for the show_engine object, a non-blocking way to script 
//   servo motor motion.
//
#ifndef Show_engine_h
#define Show_engine_h

#include <inttypes.h>

const int QUEUE_SIZE = 64;

typedef struct {
    int        prop;
    int        target;
    int        prop_interval;
    uint16_t   gesture_interval;
} move_t;

const move_t NULL_MOVE = (move_t){ prop : 0,
                                   target: 0,
                                   prop_interval: 0,
                                   gesture_interval: 0
                                 };


//---------------------------------------------------------------
class Show_engine { 
//---------------------------------------------------------------
  public:
    Show_engine(void);
    void tick(void);
    void begin(void);
    bool enqueue(move_t move);
    int dequeue(move_t* p);
    int queue_space() ;
    bool done(void);

  private:
    uint16_t timer;
    uint16_t rd_ptr;
    uint16_t wr_ptr;
    move_t   queue[QUEUE_SIZE];
};

#endif
