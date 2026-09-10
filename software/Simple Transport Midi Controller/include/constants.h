#pragma once


#define BUTTON_DEBOUNCE_TIME_MS 25U
#define TOGGLE_SWITCH_DEBOUNCE_TIME_MS 500U

#define RGB_BUTTON_FREERTOS_STACK_SIZE  8192U

#define PWR_WAKEUP_GPIO GPIO_NUM_6

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  300           // Time ESP32 will go to sleep (in seconds)


#define MIDI_CHANNEL 1

// Taken from Midi Definitions diagram
#define MIDI_CC_DAW_PLAY 22
#define MIDI_CC_DAW_PAUSE 23
#define MIDI_CC_DAW_STOP 24 // Initiates a stop and return to start of Arrangement
#define MIDI_CC_DAW_REC 25
#define MIDI_CC_DAW_QUANT 26
#define MIDI_CC_DAW_OVERDUB 27
#define MIDI_CC_DAW_METRONOME 28
#define MIDI_CC_DAW_TAP_TEMPO 29

#define MIDI_CC_INVALID 255// if =255, then nothing is added to the MIDI transport Manager queue
#define MIDI_SYSEX_TYPE_INVALID 0xFF

// Constants for how the system turns endless encoder controls into MIDI signals.
// Left = minus delta, right = plus delta. Velocity = x + start, where x in
// [0, MAX_DELTA] and x = 0 means |delta| of 1. |delta| above COARSE_THRESHOLD
// switches from the fine start value to the coarse start value.
#define MIDI_PACKET_ENDLESS_ENCODER_MAX_DELTA 10
#define MIDI_PACKET_ENDLESS_ENCODER_COARSE_THRESHOLD 3
#define MIDI_PACKET_ENDLESS_ENCODER_LEFT_COARSE_START_VAL 88
#define MIDI_PACKET_ENDLESS_ENCODER_LEFT_FINE_START_VAL 44

#define MIDI_PACKET_ENDLESS_ENCODER_RIGHT_COARSE_START_VAL 100
#define MIDI_PACKET_ENDLESS_ENCODER_RIGHT_FINE_START_VAL 22
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
// Taken from Midi Definitions Table
#ifndef INF_SCROLL_MODE_TABLE
// (name, led_colour, CC_num, sysex_msg_type)
#define INF_SCROLL_MODE_TABLE(X)                    \
    X(TEMPO_ADJUST,        PRESET_RED,   85,  0x01 ) \
    X(RECORD_QUANTISATION, PRESET_BLUE,  86,  0x02 )  \
    X(SONG_POSITION,       PRESET_GREEN, 87,  0x03 )  \
    X(ARRANGEMENT_ZOOM,    PRESET_WHITE, 88,  MIDI_SYSEX_TYPE_INVALID  )  
#endif

#ifndef VOL_ENCODER_MODE_TABLE
#define VOL_ENCODER_MODE_TABLE(X)              \
    X(MASTER_VOLUME,     PRESET_BLUE,  102)   \
    X(VOL_ENCODER_MODE2, PRESET_GREEN, 103)   \
    X(VOL_ENCODER_MODE3, PRESET_RED,   104)    \
    X(VOL_ENCODER_MODE4, PRESET_WHITE, 105)    

#endif