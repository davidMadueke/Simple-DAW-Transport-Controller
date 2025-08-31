/*
 * Rotary encoder library for Arduino. Adapted from https://github.com/maxgerhardt/rotary-encoder-over-mcp23017.git
 */

#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include "Arduino.h"

// Enable this to emit codes twice per step.
// #define HALF_STEP

// Enable weak pullups
#define ENABLE_PULLUPS

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Counter-clockwise step.
#define DIR_CCW 0x20

class RotaryEncoder
{
  public:
    RotaryEncoder(char, char);
    /* if using a rotary encoder connected directly to the MCU: call this function */
    void initPinsAndState();
    unsigned char process();
    unsigned char process(unsigned char pin1State, unsigned char pin2State);

  private:
    unsigned char state;
    unsigned char pin1;
    unsigned char pin2;
};

#endif