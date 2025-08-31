#include <INF_SCROLL_MIDI_STATE.h>
#include <VOL_ENCODER_MIDI_STATE.h>

struct  DISPLAY_STATE {
    INF_SCROLL_MODE INF_SCROLL_MODE;
    unsigned char INF_SCROLL_MODE_Value;
    
    bool VOL_POT_Event;
    VOL_ENCODER_MODE VOL_ENCODER_MODE;
    unsigned char VOL_POT_Value;

    unsigned char DAW_TEMPO;
    bool TAP_TEMPO_Event

    BATTERY_DISPLAY_STATE BATTERY_Stats;
};

struct BATTERY_DISPLAY_STATE {
    bool isLowPower;
    bool isCharging;
    bool noBatteryConnected;
    float cellPercentage;
}