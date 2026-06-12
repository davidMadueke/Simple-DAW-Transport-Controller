#include <Button.h>
#include "Arduino.h"
#include <BUTTON_MIDI_STATE.h>
#include <MIDI_Button.h>

class RecButton : public MIDI_Button
{
public:
    enum ButtonMode {
        OFF,
        ON
    };

    ButtonMode MODE = OFF;

    RecButton(uint8_t i2cAddr, uint8_t interruptPin, uint32_t dbTime,
        BUTTON_LED_STATE* toggleLedState) : MIDI_Button(i2cAddr, interruptPin, dbTime)
        {
            m_midiStateName = BUTTON_MIDI_STATE::Name::REC;
            setup_toggleLedState(toggleLedState);
            LedStateMachine(MODE);
        }

    void LedStateMachine(ButtonMode mode) {
        switch (mode) {
            case OFF:
                toggleLedOff();
                break;
            case ON:
                toggleLedOn();
                break;
        }
    }

    void stateMachine(PushButtonEvent* event) override {
        if (event != nullptr) {

            ButtonMode nextMode = MODE;
            switch (MODE) {
                case OFF:
                    if (event->type == PushButtonEvent::Type::RegularPressComplete) {
                        nextMode = ON;
                        vPostEvent(m_midiStateName, BUTTON_MIDI_STATE::Type::RegularPress, 1);
                    }
                    break;

                case ON:
                    if (event->type == PushButtonEvent::Type::RegularPressComplete) {
                        nextMode = OFF;
                        vPostEvent(m_midiStateName, BUTTON_MIDI_STATE::Type::RegularPress, 1);
                    }
                    break;
            }

            if (nextMode != MODE) {
                MODE = nextMode;
                LedStateMachine(MODE);
            }
        }
    }
};
