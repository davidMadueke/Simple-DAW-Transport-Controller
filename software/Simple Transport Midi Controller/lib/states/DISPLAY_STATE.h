#include <Arduino.h>
#include <INF_SCROLL_MIDI_STATE.h>
#include <VOL_ENCODER_MIDI_STATE.h>
#include <BLUETOOTH_MIDI_STATE.h>

#pragma once

struct BATTERY_DISPLAY_STATE {
    bool isLowPower;
    bool isCharging;
    bool noBatteryConnected;
    float cellPercentage;
};
struct  DISPLAY_STATE {
    uint32_t version = 0;
    INF_SCROLL_MODE infScrollMode;
    unsigned char infScrollMode_value;
    
    VOL_ENCODER_MODE volEncoderMode;
    unsigned char volEncoder_potValue;

    unsigned char dawTempo;
    bool tapTempoEvent;

    BATTERY_DISPLAY_STATE batteryState;

    BLUETOOTH_MIDI_STATE bleFlags;

    bool dirty(const DISPLAY_STATE& other) const { return version != other.version; }
};


extern DISPLAY_STATE g_displayState;
extern portMUX_TYPE g_displayStateMux;