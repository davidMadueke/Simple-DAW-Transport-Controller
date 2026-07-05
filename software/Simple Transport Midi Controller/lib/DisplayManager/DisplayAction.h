#pragma once
#include <Arduino.h>
#include <MIDI_PACKET.h>

struct DISPLAY_ACTION {
    enum TYPE {
    TAP_TEMPO,
    DAW_TEMPO_CHANGED,
    BATTERY_LOW_PWR,
    INF_SCROLL_MODE_INCREMENTED,
    VOL_ENCODER_MODE_INCREMENTED,
    BLE_CONNECTED, 
    BUTTON_PRESS
    } type;       

    MIDI_PACKET midi_packet = {};
};

DISPLAY_ACTION createButtonPressAction(MIDI_PACKET packet)
{
    DISPLAY_ACTION a;
    a.type = DISPLAY_ACTION::TYPE::BUTTON_PRESS;
    a.midi_packet = packet;
    return a;
}

DISPLAY_ACTION createTapTempoAction(MIDI_PACKET packet)
{
    DISPLAY_ACTION a;
    a.type = DISPLAY_ACTION::TYPE::TAP_TEMPO;
    a.midi_packet = packet;
    return a;
}