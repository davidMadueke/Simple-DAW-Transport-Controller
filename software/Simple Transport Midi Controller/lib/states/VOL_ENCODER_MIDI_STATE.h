#pragma once
#include <Arduino.h>
#include <HAL_RotaryEncoder.h>
#if __has_include("constants.h")
    #include "constants.h"
#endif

// Fallback so this header still compiles if constants.h didn't define the table
// (e.g. a standalone unit-test translation unit). The authoritative copy lives
// in constants.h.  X(mode_name, led_preset, midi_cc)

#ifndef VOL_ENCODER_MODE_TABLE
#define VOL_ENCODER_MODE_TABLE(X)                                     \
    X(MODE1, VOL_ENCODER_LED_RING_BLUE,  MIDI_CC_VOL_ENCODER_MODE1)   \
    X(MODE2, VOL_ENCODER_LED_RING_GREEN, MIDI_CC_VOL_ENCODER_MODE2)   \
    X(MODE3, VOL_ENCODER_LED_RING_RED,   MIDI_CC_VOL_ENCODER_MODE3)
#endif


enum VOL_ENCODER_MODE
{
    VOL_ENCODER_MODE_NONE,
    #define X(name, led, cc, ...) name,
        VOL_ENCODER_MODE_TABLE(X)
    #undef X
    VOL_ENCODER_MODE_COUNT
};

struct VOL_ENCODER_MIDI_STATE {

    VOL_ENCODER_MODE MODE = VOL_ENCODER_MODE::VOL_ENCODER_MODE_NONE;
    RotaryEncoderEvent event = {};
};
