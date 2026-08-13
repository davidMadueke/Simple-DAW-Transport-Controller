#pragma once
#include <Arduino.h>
#include <HAL_RotaryEncoder.h>
#if __has_include("constants.h")
    #include "constants.h"
#endif

// Fallback so this header still compiles if constants.h didn't define the table
// (e.g. a standalone unit-test translation unit). The authoritative copy lives
// in constants.h.  X(mode_name, led_preset, midi_cc)

#ifndef INF_SCROLL_MODE_TABLE
#define INF_SCROLL_MODE_TABLE(X)                                                            \
    X(TEMPO_ADJUST,        INF_SCROLL_LED_RED,  MIDI_CC_INF_SCROLL_TEMPO_ADJUST) \
    X(RECORD_QUANTISATION, INF_SCROLL_LED_BLUE, MIDI_CC_INF_SCROLL_RECORD_QUANTISATION)
#endif

enum INF_SCROLL_MODE
{
    INF_SCROLL_MODE_NONE,
#define X(name, led, cc, ...) name,
    INF_SCROLL_MODE_TABLE(X)
#undef X
    INF_SCROLL_MODE_COUNT
};

struct INF_SCROLL_MIDI_STATE
{
    INF_SCROLL_MODE MODE = INF_SCROLL_MODE::TEMPO_ADJUST;
    RotaryEncoderEvent event = {};
};
