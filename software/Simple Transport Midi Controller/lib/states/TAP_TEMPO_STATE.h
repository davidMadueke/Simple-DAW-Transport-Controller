#include <BUTTON_MIDI_STATE.h>

struct TAP_TEMPO_STATE{
    bool tapTempoEvent;
};

void tapTempoEventCheck(TAP_TEMPO_STATE* TEMPO, BUTTON_MIDI_STATE* MIDI) {
    if (TEMPO->tapTempoEvent) { 
        MIDI->tapTempoButton_PressEvent = true;
    };

    // This is to ensure that once the INPUT manager asserts the MIDI state as false, the TAP TEMPO STATE follows accordingly
    if (!MIDI->tapTempoButton_PressEvent){
        TEMPO->tapTempoEvent = false;
    }
}