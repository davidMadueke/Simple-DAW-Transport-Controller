#pragma once

struct MIDI_PACKET {
    enum class TYPE : uint8_t  { // Affix enums to be one byte each
    NOTE_ON,
    NOTE_OFF,
    CC,
    PITCH_BEND,
    } type;       // Type of message (Note On, CC, Pitch Bend)
    uint8_t data1;      // Note number or CC number
    uint8_t data2;      // Velocity or CC value
    uint8_t channel;    // MIDI Channel (1-16)
};

