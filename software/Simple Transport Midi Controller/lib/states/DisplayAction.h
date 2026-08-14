#pragma once
#include <Arduino.h>
#include <MIDI_PACKET.h>
#include <VOL_ENCODER_MIDI_STATE.h>
#include <INF_SCROLL_MIDI_STATE.h>

struct DISPLAY_ACTION {
    enum TYPE {
    TAP_TEMPO,
    DAW_TEMPO_CHANGED,
    BATTERY_LOW_PWR,
    INF_SCROLL_MODE_INCREMENTED,
    INF_SCROLL_DELTA,
    VOL_ENCODER_MODE_INCREMENTED,
    VOL_ENCODER_POSITION,
    BLE_CONNECTED, 
    BUTTON_PRESS
    } type;       

    MIDI_PACKET midi_packet = {};
    VOL_ENCODER_MODE vol_encoder_mode = VOL_ENCODER_MODE::VOL_ENCODER_MODE_NONE;
    INF_SCROLL_MODE inf_scroll_mode = INF_SCROLL_MODE::INF_SCROLL_MODE_NONE;
    int16_t inf_scroll_encoder_delta = 0;
};

inline DISPLAY_ACTION createButtonPressAction(MIDI_PACKET packet)
{
    DISPLAY_ACTION a;
    a.type = DISPLAY_ACTION::TYPE::BUTTON_PRESS;
    a.midi_packet = packet;
    return a;
}

inline DISPLAY_ACTION createTapTempoAction(MIDI_PACKET packet)
{
    DISPLAY_ACTION a;
    a.type = DISPLAY_ACTION::TYPE::TAP_TEMPO;
    a.midi_packet = packet;
    return a;
}

inline DISPLAY_ACTION createVolEncoderPositionAction(MIDI_PACKET packet)
{
    DISPLAY_ACTION a;
    a.type = DISPLAY_ACTION::TYPE::VOL_ENCODER_POSITION;
    if(packet.data1 != MIDI_CC_INVALID)
    {
        a.midi_packet = packet;
    }
    
    return a;
}

inline DISPLAY_ACTION createVolEncoderModeAction(VOL_ENCODER_MODE mode)
{
    DISPLAY_ACTION a;
    a.type = DISPLAY_ACTION::TYPE::VOL_ENCODER_MODE_INCREMENTED;
    a.vol_encoder_mode = mode;
    return a;
}

inline DISPLAY_ACTION createInfScrollModeAction(INF_SCROLL_MODE mode)
{
    DISPLAY_ACTION a;
    a.type = DISPLAY_ACTION::TYPE::INF_SCROLL_MODE_INCREMENTED;
    a.inf_scroll_mode = mode;
    return a;
}

inline DISPLAY_ACTION createInfScrollEncoderDeltaAction(MIDI_PACKET packet)
{
    DISPLAY_ACTION a;
    a.type = DISPLAY_ACTION::TYPE::INF_SCROLL_DELTA;
    if(packet.data1 != MIDI_CC_INVALID)
    {
        a.midi_packet = packet;
    }
    
    return a;
}