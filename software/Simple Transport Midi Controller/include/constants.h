#pragma once


#define BUTTON_DEBOUNCE_TIME_MS 25U
#define TOGGLE_SWITCH_DEBOUNCE_TIME_MS 500U

#define RGB_BUTTON_FREERTOS_STACK_SIZE  8192U

#define PWR_WAKEUP_GPIO GPIO_NUM_6

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  300           // Time ESP32 will go to sleep (in seconds)


#define MIDI_CHANNEL 1

#define MIDI_CC_DAW_PLAY 22
#define MIDI_CC_DAW_PAUSE 23
#define MIDI_CC_DAW_STOP 24 // Initiates a stop and return to start of Arrangement
#define MIDI_CC_DAW_REC 25
#define MIDI_CC_DAW_QUANT 26
#define MIDI_CC_DAW_OVERDUB 27
#define MIDI_CC_DAW_METRONOME 28
#define MIDI_CC_DAW_TAP_TEMPO 29

#define MIDI_CC_INVALID 255// if =255, then nothing is added to the MIDI transport Manager queue

// INF_SCROLL and VOL_ENC encoder buttons do not transmit an MIDI information
// Rather, they just change its respective Encoder Modes



// ---------------------------------------------------------------------------
// Mode tables: the single source of truth for each encoder's modes.
//   X(mode_name, led_preset, midi_cc)
// The led_preset tokens are defined in the encoder headers and are only
// expanded at the LED switch call-site (where they are in scope), so it is safe
// to reference them here. The matching ..._MIDI_STATE.h headers provide an
// #ifndef fallback copy for standalone/unit-test compilation.
// ---------------------------------------------------------------------------



//#define INF_SCROLL_MODE_TABLE_ENABLE_EXTERNAL_DEF 1
#ifndef INF_SCROLL_MODE_TABLE
#define INF_SCROLL_MODE_TABLE(X)                    \
    X(TEMPO_ADJUST,        PRESET_RED,   8) \
    X(RECORD_QUANTISATION, PRESET_BLUE,  9)
#endif

#ifndef VOL_ENCODER_MODE_TABLE
#define VOL_ENCODER_MODE_TABLE(X)              \
    X(MASTER_VOLUME, PRESET_BLUE,  10)   \
    X(VOL_ENCODER_MODE2, PRESET_GREEN, 11)   \
    X(VOL_ENCODER_MODE3, PRESET_RED,   12)    \
    X(VOL_ENCODER_MODE4, PRESET_WHITE, 13)    

#endif