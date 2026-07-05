#include <Arduino.h>
#include <HAL_RotaryEncoder.h>

#pragma once

enum INF_SCROLL_MODE 
{
    NONE,
    //ARRANGEMENT_ZOOM;
    //PLAYBACK_ADJUST;
    TEMPO_ADJUST,
    RECORD_QUANTISATION,
    INF_SCROLL_MODE_COUNT
};

struct INF_SCROLL_MIDI_STATE
{
    INF_SCROLL_MODE MODE = INF_SCROLL_MODE::TEMPO_ADJUST;
    RotaryEncoderEvent event = {};
};

struct INF_SCROLL_RGB_LED_STATE
{
    uint8_t RED;
    uint8_t GREEN;
    uint8_t BLUE;
};
