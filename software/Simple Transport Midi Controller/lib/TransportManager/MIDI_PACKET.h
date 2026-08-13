#pragma once
#include <Arduino.h>

// Payload only (excludes SysEx start, manufacturer ID, and SysEx end).
// Matches FortySevenEffects DefaultSettings::SysExMaxSize (128) minus 3 framing bytes.
#ifndef MIDI_PACKET_SYSEX_MAX_PAYLOAD
    #define MIDI_PACKET_SYSEX_MAX_PAYLOAD 125u
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

    unsigned sysexSize = 0; // Count of bytes in sysexData (payload only)
    uint8_t sysexData[MIDI_PACKET_SYSEX_MAX_PAYLOAD] = {};
};
