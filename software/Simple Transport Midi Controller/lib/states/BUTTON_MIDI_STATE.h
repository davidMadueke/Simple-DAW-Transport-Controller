#include <Arduino.h>
#ifndef BUTTON_MIDI_STATE_H
#define BUTTON_MIDI_STATE_H
// struct BUTTON_MIDI_STATE
// {
//     BUTTON_HAL_STATE playButton;
//     BUTTON_HAL_STATE pauseButton;
//     BUTTON_HAL_STATE recButton;
//     BUTTON_HAL_STATE overdubButton;
//     BUTTON_HAL_STATE quantiseButton;
//     BUTTON_HAL_STATE metronomeButton;
//     bool tapTempoButton_PressEvent; // This will be used for both the when the metronome BTN is in Tap Tempo mode and the Tap Tempo footswitch Input
// };

struct BUTTON_MIDI_STATE
{
    enum class Name : uint8_t {
        PLAY,
        STOP,
        REC,
        QUANT,
        OVERDUB,
        METRONOME,
        TAP_TEMPO,
    } name;

    enum class Type : uint8_t {
        RegularPress,
        LongPress,
    } type;

    
    uint8_t pressCount;
};

struct BUTTON_LED_STATE
{
    uint8_t Red;
    uint8_t Blue;
    uint8_t Green;
};

#endif