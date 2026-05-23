/* Rotary encoder handler for arduino.
 *
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 * Contact: bb@cactii.net
 *
 */

#include "Arduino.h"
#include "HAL_RotaryEncoder.h"


/*
 * Constructor. Each arg is the pin number for each encoder contact.
 */
HAL_RotaryEncoder::HAL_RotaryEncoder(uint8_t enc_pinA, uint8_t enc_pinB, uint8_t sw_pinAddr, uint32_t sw_dbTime) {
  // Assign variables.
  m_enc_pinA = enc_pinA;
  m_enc_pinB = enc_pinB;
  m_sw_pin = sw_pinAddr;
  m_sw_dbTime = sw_dbTime;
  encoder = new ESP_Knob(enc_pinA, enc_pinB);
  sw = new PushButton(m_sw_pin, m_sw_dbTime);
}

void HAL_RotaryEncoder::begin() {
  encoder->begin();
}

