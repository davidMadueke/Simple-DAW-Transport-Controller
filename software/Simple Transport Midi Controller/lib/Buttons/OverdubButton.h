#include <Button.h>
#include "Arduino.h"
#include <BUTTON_MIDI_STATE.h>
#include <MIDI_Button.h>

class OverdubButton : public MIDI_Button {
    public:

        OverdubButton(
            uint8_t i2cAddr, uint32_t dbTime,
            BUTTON_LED_STATE* singlePressState
        ) : MIDI_Button(i2cAddr, dbTime)
        {
            setup_singlePressLedIndicator(singlePressState);
        };

        void stateMachine(bool ISR_State, BUTTON_HAL_STATE* HAL) override 
        {
            uint8_t numOfPresses = button->multiPressRead(ISR_State);
            if (numOfPresses > 0){ HAL->buttonPressEvent = true; } // We will need to handle turning off the press event as soon as MIDI msg is compiled
            
        };
};