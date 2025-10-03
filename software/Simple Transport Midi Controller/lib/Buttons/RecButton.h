#include <Button.h>
#include "Arduino.h"
#include <BUTTON_MIDI_STATE.h>
#include <MIDI_Button.h>


class RecButton : public MIDI_Button
{
    public:
        RecButton(uint8_t i2cAddr, uint32_t dbTime, 
            BUTTON_LED_STATE* toggleLedState) : MIDI_Button(i2cAddr, dbTime) 
            {
                setup_toggleLedState(toggleLedState);
            }

        void begin();
        enum ButtonMode {
            ON,
            OFF
        };
        ButtonMode MODE = ON;

        void stateMachine(bool ISR_State, BUTTON_HAL_STATE* HAL) override {

            switch (MODE)
            {
                case OFF:
                    ledOff();

                    if (button->read(ISR_State)){
                        HAL->buttonPressEvent = true;
                        HAL->numOfPresses = 1;
                        MODE = ON;
                    }
                    break;
                    
                case ON:
                    toggleLedOn();

                    if (button->read(ISR_State)){
                            HAL->buttonPressEvent = true;
                            HAL->numOfPresses = 1;
                            MODE = OFF;
                        }
                    break;
            
            }
        };
} ;  