/*
 * Rotary encoder library for Arduino. Adapted from https://github.com/maxgerhardt/rotary-encoder-over-mcp23017.git
 */

#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include "Arduino.h"
#include <ESP_KNOB.h>
#include <PushButton.h>
class RotaryEncoder
{
  public:
 
    RotaryEncoder(uint8_t enc_pinA, uint8_t enc_pinB, 
      uint8_t sw_pinAddr, 
      uint32_t sw_dbTime);
      /*To initialise both the EC11 Encoder and the Pushbutton (Rotary Encoder Switch)*/
      void begin();

    /**
   * @brief Attach the knob left callback function
   * @param callback Callback function to be attached
   */
  void attachLeftEventCallback(std::function<void(int, void *)> callback);

  /**
   * @brief Detach the knob left callback function
   */
  void detachLeftEventCallback(void);

  /**
   * @brief Attach the knob right callback function
   * @param callback Callback function to be attached
   */
  void attachRightEventCallback(std::function<void(int, void *)> callback);

  /**
   * @brief Detach the knob right callback function
   */
  void detachRightEventCallback(void);

  int getCount() {
    // Return the count value from the ESP_Knob object.
    // Note: The count value on the ESP_Knob increases by 2 for every click.
    return encoder->getCountValue();
  }

  int getPrevEspCount(){
    return m_PREV_ESP_COUNT;
  }

  void setPrevEspCount(int count){
    m_PREV_ESP_COUNT = count;
  }

  void clearCount() {
    // Clear the count value in the ESP_Knob object.
    encoder->clearCountValue();
  }

  bool sw_wasPressed();

  bool sw_read(bool State);
    
  private:
    ESP_Knob *encoder;
    PushButton *sw;

    int encoderCount;
    unsigned char m_enc_pinA;
    unsigned char m_enc_pinB;
    uint8_t m_sw_pin;
    uint32_t m_sw_dbTime; // Debounce Time

    int m_PREV_ESP_COUNT = 0;
};

#endif