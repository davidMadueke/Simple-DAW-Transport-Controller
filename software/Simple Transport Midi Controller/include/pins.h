#pragma once

#ifndef PINS_H
#define PINS_H

// MCP23017 Pin Declarations
// GPA pins (Port A)
#define MCP_GPA0 0
#define MCP_GPA1 1
#define MCP_GPA2 2
#define MCP_GPA3 3
#define MCP_GPA4 4
#define MCP_GPA5 5
#define MCP_GPA6 6
#define MCP_GPA7 7

// GPB pins (Port B)
#define MCP_GPB0 8
#define MCP_GPB1 9
#define MCP_GPB2 10
#define MCP_GPB3 11
#define MCP_GPB4 12
#define MCP_GPB5 13
#define MCP_GPB6 14
#define MCP_GPB7 15

// Pin declarations for the MCP23017 INTA INTB interrupt pins sent to the ESP32
#define MCP_INTA 14
#define MCP_INTB 32

// Encoders Pin declarations
#define PIN_ENC_INF_SCROLL_R 0
#define PIN_ENC_INF_SCROLL_G 1
#define PIN_ENC_INF_SCROLL_B 2
#define PIN_ENC_INF_SCROLL_W1 MCP_GPA0
#define PIN_ENC_INF_SCROLL_W2 MCP_GPA1
#define PIN_ENC_INF_SCROLL_SW MCP_GPB2

#define PIN_ENC_VOLUME_W1 MCP_GPA2
#define PIN_ENC_VOLUME_W2 MCP_GPA3
#define PIN_ENC_VOLUME_SW MCP_GPB1

// Button Interrupts Pins
#define PIN_BTN_PLAY_INT 15
#define PIN_BTN_STOP_INT 33
#define PIN_REC_INT 27
#define PIN_BTN_QUANTISE_INT 37
#define PIN_BTN_METRONOME_INT 3
#define PIN_BTN_OVERDUB_INT 4

// TAP Tempo Jack Inputs
#define PIN_JACK_TAP_TEMPO MCP_GPB0


#endif /* PINS_H */