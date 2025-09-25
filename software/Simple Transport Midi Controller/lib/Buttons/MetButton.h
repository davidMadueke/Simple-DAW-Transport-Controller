#include <Button.h>
#include "Arduino.h"
#include <BUTTON_MIDI_STATE.h>
#include <TAP_TEMPO_STATE.h>
#include <MIDI_Button.h>

// Object that describes lower level features of the Metronome (Tap Tempo) button
class MetButton : public MIDI_Button
{
    private:
    TAP_TEMPO_STATE* s_TEMPO;
public:
    enum ButtonMode {
        METRONOME_OFF,
        METRONOME_ON,
        toLONG_PRESS,
        LONG_PRESS,
        toMETRONOME_OFF
    };
    
    MetButton(uint8_t i2cAddr, uint32_t dbTime, BUTTON_LED_STATE* longPressState, 
        BUTTON_LED_STATE* singlePressState, BUTTON_LED_STATE* toggleLedState, TAP_TEMPO_STATE* tapTempoState,
        uint8_t timeForLongPress = 100)
        : MIDI_Button(i2cAddr, dbTime) 
        {
            s_TEMPO = tapTempoState;
            setup_singlePressLedIndicator(singlePressState);
            setup_longPress(longPressState, timeForLongPress);
            setup_toggleLedState(toggleLedState);
        };

    // Override stateMachine
    void stateMachine(bool ISR_State, BUTTON_HAL_STATE* HAL) override {
        static ButtonMode MODE;
        uint8_t numOfPresses = button->multiPressRead(ISR_State);
        if (numOfPresses > 0){ HAL->buttonPressEvent = true; } // We will need to handle turning off the press event as soon as MIDI msg is compiled

        switch (MODE) {
            case METRONOME_OFF:
                if (numOfPresses > 0){
                        HAL->numOfPresses = numOfPresses;
                        MODE = METRONOME_ON;
                    }
                    break;

                if (button->pressedFor(getTimeForLongPress())) {
                    MODE = toLONG_PRESS;
                    HAL->longPress = true;
                    HAL->numOfPresses = 1;
                }
                break;
            
            case METRONOME_ON:
                toggleLedOn();

                if (numOfPresses > 0){
                        HAL->numOfPresses = numOfPresses;
                        MODE = METRONOME_OFF;
                    }
                    
                if (button->pressedFor(getTimeForLongPress())) {
                    MODE = toLONG_PRESS;
                    HAL->longPress = true;
                    HAL->numOfPresses = 1;
                }
                break;

            case toLONG_PRESS:
                toggleLedOn();
                if (button->wasReleased())
                    MODE = LONG_PRESS;
                break;

            case LONG_PRESS:
                if (button->pressedFor(getTimeForLongPress())) {
                    HAL->longPress = false;
                    MODE = toMETRONOME_OFF;
                }
                else {
                    HAL->longPress = true;
                    HAL->numOfPresses = numOfPresses;

                    s_TEMPO->tapTempoEvent = true;
                    if (!button->isPressed()) {
                        setLedColour(longPressLedState);
                    }
                }
                break;

            case toMETRONOME_OFF:
                if (button->wasReleased())
                    MODE = METRONOME_OFF;
                break;
        }
    }
};
