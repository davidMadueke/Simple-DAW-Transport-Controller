#pragma once
#define BUTTON_DEBOUNCE_TIME_MS 25
#define TOGGLE_SWITCH_DEBOUNCE_TIME_MS 500

#define RGB_BUTTON_FREERTOS_STACK_SIZE  8192

#define PWR_WAKEUP_GPIO GPIO_NUM_6

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  300           // Time ESP32 will go to sleep (in seconds)

#define MIDI_CC_PLAY_BTN 1
#define MIDI_CC_STOP_BTN 2
#define MIDI_CC_REC_BTN 3
#define MIDI_CC_QUANT_BTN 4
#define MIDI_CC_OVERDUB_BTN 5
#define MIDI_CC_TAP_TEMPO 7

// INF_SCROLL and VOL_ENC encoder buttons do not transmit an MIDI information
// Rather, they just change its respective Encoder Modes

#define MIDI_CC_INF_SCROLL_TEMPO_ADJUST 8
#define MIDI_CC_INF_SCROLL_RECORD_QUANTISATION 9