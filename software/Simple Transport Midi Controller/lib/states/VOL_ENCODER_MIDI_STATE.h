#include <Arduino.h>
#include <HAL_RotaryEncoder.h>
#include <LedRing_DUPPA.h>

#pragma once

enum VOL_ENCODER_MODE 
{
    MODE1,
    MODE2,
    MODE3,
};

struct VOL_ENCODER_MIDI_STATE {
    
    VOL_ENCODER_MODE MODE = VOL_ENCODER_MODE::MODE1;
    LED_RING_DIAL_STATE ledRingState;
    RotaryEncoderEvent event = {};
};

