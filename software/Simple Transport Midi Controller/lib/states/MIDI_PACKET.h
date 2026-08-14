#pragma once
#include <Arduino.h>
#if __has_include("constants.h")
    #include "constants.h"
#endif

// Payload only (excludes SysEx start, manufacturer ID, and SysEx end).
// Matches FortySevenEffects DefaultSettings::SysExMaxSize (128) minus 3 framing bytes.
#ifndef MIDI_PACKET_SYSEX_MAX_PAYLOAD
    #define MIDI_PACKET_SYSEX_MAX_PAYLOAD 125u
#endif

#ifndef MIDI_PACKET_ENDLESS_ENCODER_MAX_DELTA
    #define MIDI_PACKET_ENDLESS_ENCODER_MAX_DELTA 10
#endif

#ifndef MIDI_PACKET_ENDLESS_ENCODER_COARSE_THRESHOLD
    #define MIDI_PACKET_ENDLESS_ENCODER_COARSE_THRESHOLD 3
#endif

#ifndef MIDI_PACKET_ENDLESS_ENCODER_LEFT_COARSE_START_VAL
    #define MIDI_PACKET_ENDLESS_ENCODER_LEFT_COARSE_START_VAL 88
#endif

#ifndef MIDI_PACKET_ENDLESS_ENCODER_LEFT_FINE_START_VAL
    #define MIDI_PACKET_ENDLESS_ENCODER_LEFT_FINE_START_VAL 44
#endif

#ifndef MIDI_PACKET_ENDLESS_ENCODER_RIGHT_COARSE_START_VAL
    #define MIDI_PACKET_ENDLESS_ENCODER_RIGHT_COARSE_START_VAL 100
#endif

#ifndef MIDI_PACKET_ENDLESS_ENCODER_RIGHT_FINE_START_VAL
    #define MIDI_PACKET_ENDLESS_ENCODER_RIGHT_FINE_START_VAL 22
#endif


struct MIDI_PACKET {
    enum TYPE : uint8_t { // Affix enums to be one byte each
        NOTE_ON,
        NOTE_OFF,
        CC,
        PITCH_BEND,
        ENDLESS_ENCODER,
        SYSEX
    } type;                 // Type of message (Note On, CC, Pitch Bend, ...)

    uint8_t data1 = 0;      // Note number or CC number
    uint8_t data2 = 0;      // Velocity or CC value
    uint8_t channel = 1;    // MIDI Channel (1-16)

    enum INTERFACE : uint8_t {
        ANY,                // Default for outbound / unspecified
        USB,
        SER,
        BLE
    } interface = INTERFACE::ANY;

    bool isCoarse = false; // For Endless Encoder Mode, if true it is using the Coarse range of CC Value Space

    unsigned sysexSize = 0; // Count of bytes in sysexData (payload only)
    uint8_t sysexData[MIDI_PACKET_SYSEX_MAX_PAYLOAD] = {};
};

// Encodes HAL rotary-encoder acceleration as an Endless Encoder MIDI Note On:
// data1 = note number, data2 = velocity. Left (minus delta) / right (plus delta)
// pick the start value; |delta| > COARSE_THRESHOLD switches fine -> coarse.
// x is |delta|-1, clamped to [0, MAX_DELTA], so x = 0 means a delta of 1.
inline MIDI_PACKET createEndlessEncoderPacket(uint8_t noteNumber, int16_t encoderDelta, uint8_t channel = 1)
{
    MIDI_PACKET packet{};
    packet.type = MIDI_PACKET::TYPE::ENDLESS_ENCODER;
    packet.data1 = noteNumber;
    packet.channel = channel;

    const int16_t magnitude = (encoderDelta < 0) ? (int16_t)(-encoderDelta) : encoderDelta;
    const bool isCoarse = magnitude > MIDI_PACKET_ENDLESS_ENCODER_COARSE_THRESHOLD;
    packet.isCoarse = isCoarse;
    const uint8_t x = (uint8_t)constrain(magnitude - 1, 0, MIDI_PACKET_ENDLESS_ENCODER_MAX_DELTA);

    uint8_t startValue;
    if (encoderDelta < 0)
    {
        startValue = isCoarse
            ? MIDI_PACKET_ENDLESS_ENCODER_LEFT_COARSE_START_VAL
            : MIDI_PACKET_ENDLESS_ENCODER_LEFT_FINE_START_VAL;
    }
    else
    {
        startValue = isCoarse
            ? MIDI_PACKET_ENDLESS_ENCODER_RIGHT_COARSE_START_VAL
            : MIDI_PACKET_ENDLESS_ENCODER_RIGHT_FINE_START_VAL;
    }

    packet.data2 = (uint8_t)constrain((int16_t)startValue + (int16_t)x, 0, 127);
    return packet;
}