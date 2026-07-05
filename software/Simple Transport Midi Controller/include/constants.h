#pragma once
#define BUTTON_DEBOUNCE_TIME_MS 25U
#define TOGGLE_SWITCH_DEBOUNCE_TIME_MS 500U

#define RGB_BUTTON_FREERTOS_STACK_SIZE  8192U 

#define PWR_WAKEUP_GPIO GPIO_NUM_6

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  300           // Time ESP32 will go to sleep (in seconds)


#define MIDI_CHANNEL 1

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

// ---------------------------------------------------------------------------
// InfScrollEncoder FreeRTOS task configuration
// ---------------------------------------------------------------------------
#ifndef INF_SCROLL_ENCODER_FREERTOS_PRIORITY
    #define INF_SCROLL_ENCODER_FREERTOS_PRIORITY 3
#endif

#ifndef INF_SCROLL_ENCODER_FREERTOS_TASK_STACK_SIZE
    #define INF_SCROLL_ENCODER_FREERTOS_TASK_STACK_SIZE 4096
#endif

#ifndef INF_SCROLL_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH
    #define INF_SCROLL_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH 8
#endif

#ifndef INF_SCROLL_ENCODER_QUEUE_RECEIVE_TIMEOUT_MS
    #define INF_SCROLL_ENCODER_QUEUE_RECEIVE_TIMEOUT_MS 200
#endif

// ---------------------------------------------------------------------------
// InfScrollEncoder per-mode LED colours.
// These reference the named colour presets defined in InfScrollEncoder.h.
// ---------------------------------------------------------------------------
#ifndef INF_SCROLL_LED_TEMPO_ADJUST
    #define INF_SCROLL_LED_TEMPO_ADJUST        INF_SCROLL_LED_BLUE
#endif

#ifndef INF_SCROLL_LED_RECORD_QUANTISATION
    #define INF_SCROLL_LED_RECORD_QUANTISATION INF_SCROLL_LED_TEST
#endif

#define INF_SCROLL_LED_BRIGHTNESS_PRESCALER 2