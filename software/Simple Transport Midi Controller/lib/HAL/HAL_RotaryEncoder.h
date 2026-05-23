
#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include "Arduino.h"
#include <RotaryEncoder.h> // Matthias Hertel Rotary Encoder Lib
#include <PushButton.h>

class HAL_RotaryEncoder
{
  public:
 
    HAL_RotaryEncoder(uint8_t enc_pinA, uint8_t enc_pinB, 
      uint8_t sw_pinAddr, 
      uint32_t sw_dbTime);
      /*To initialise both the EC11 Encoder and the Pushbutton (Rotary Encoder Switch)*/
      void begin();

    
  private:
    RotaryEncoder* encoder;
    PushButton *sw;

    int encoderCount;
    unsigned char m_enc_pinA;
    unsigned char m_enc_pinB;
    uint8_t m_sw_pin;
    uint32_t m_sw_dbTime; // Debounce Time

    int m_PREV_ESP_COUNT = 0;
};

#endif