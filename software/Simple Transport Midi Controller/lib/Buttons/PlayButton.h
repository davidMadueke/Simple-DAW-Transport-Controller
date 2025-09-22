#include <Button.h>
#include "Arduino.h"
#include <BUTTON_MIDI_STATE.h>
#include <MIDI_Button.h>



// PlayButton now inherits from MIDI_Button
class PlayButton : public MIDI_Button
{
public:
    enum ButtonMode {
        SINGLE_PRESS,
        toLONG_PRESS,
        LONG_PRESS,
        toSINGLE_PRESS
    };
    
    PlayButton(uint8_t i2cAddr, uint32_t dbTime, BUTTON_LED_STATE* longPressState, 
        BUTTON_LED_STATE* singlePressState, uint8_t timeForLongPress = 100)
        : MIDI_Button(i2cAddr, dbTime) 
        {
            setup_singlePressLedIndicator(singlePressState);
            setup_longPress(longPressState, timeForLongPress);
        };

    // Override stateMachine
    void stateMachine(bool ISR_State, BUTTON_HAL_STATE* HAL) override {
        static ButtonMode MODE;
        uint8_t numOfPresses = button->multiPressRead(ISR_State);
        if (numOfPresses > 0){ HAL->buttonPressEvent = true; } // We will need to handle turning off the press event as soon as MIDI msg is compiled

        switch (MODE) {
            case SINGLE_PRESS:
                HAL->numOfPresses = numOfPresses;
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
                    MODE = toSINGLE_PRESS;
                }
                else {
                    HAL->longPress = true;
                    HAL->numOfPresses = numOfPresses;
                    if (!button->isPressed()) {
                        setLedColour(longPressLedState);
                    }
                }
                break;
            case toSINGLE_PRESS:
                if (button->wasReleased())
                    MODE = SINGLE_PRESS;
                break;
        }
    }
};
