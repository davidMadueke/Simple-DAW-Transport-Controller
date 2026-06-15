#pragma once
#include <Arduino.h>

struct DisplayAction {
    enum TYPE { 
    TAP_TEMPO,
    DAW_TEMPO_CHANGED,
    BATTERY_LOW_PWR,
    INF_SCROLL_MODE,
    VOL_ENCODER_MODE,
    BLE_CONNECTED
    } type;       
    uint8_t data1;      // Note number or CC number
    uint8_t data2;      // Velocity or CC value
    uint8_t channel;    // MIDI Channel (1-16)
};