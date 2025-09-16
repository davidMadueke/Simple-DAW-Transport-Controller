/* Rotary encoder handler for arduino.
 *
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 * Contact: bb@cactii.net
 *
 */

#include "Arduino.h"
#include "RotaryEncoder.h"


/*
 * Constructor. Each arg is the pin number for each encoder contact.
 */
RotaryEncoder::RotaryEncoder(uint8_t enc_pinA, uint8_t enc_pinB, uint8_t sw_pinAddr, uint32_t sw_dbTime) {
  // Assign variables.
  m_enc_pinA = enc_pinA;
  m_enc_pinB = enc_pinB;
  m_sw_pin = sw_pinAddr;
  m_sw_dbTime = sw_dbTime;
  encoder = new ESP_Knob(enc_pinA, enc_pinB);
  sw = new PushButton(m_sw_pin, m_sw_dbTime);
}

void RotaryEncoder::begin() {
  encoder->begin();
}

void RotaryEncoder::attachLeftEventCallback(std::function<void(int, void *)> callback) {
  // Attach the given callback function to the left event in the ESP_Knob
  // object.
  encoder->attachLeftEventCallback(callback);
}

void RotaryEncoder::detachLeftEventCallback(void) {
  // Detach the callback function from the left event in the ESP_Knob object.
  encoder->detachLeftEventCallback();
}

void RotaryEncoder::attachRightEventCallback(std::function<void(int, void *)> callback) {
  // Attach the given callback function to the right event in the ESP_Knob
  // object.
  encoder->attachRightEventCallback(callback);


}

void RotaryEncoder::detachRightEventCallback(void) {
  // Detach the callback function from the right event in the ESP_Knob object.
  encoder->detachRightEventCallback();
}

bool RotaryEncoder::sw_read(bool State){
  return sw->read(State);
}
bool RotaryEncoder::sw_wasPressed(){
  return sw->wasPressed();
}