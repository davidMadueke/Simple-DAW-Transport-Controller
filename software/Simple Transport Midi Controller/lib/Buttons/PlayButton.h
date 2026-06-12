#include <Button.h>
#include "Arduino.h"
#include <BUTTON_MIDI_STATE.h>
#include <MIDI_Button.h>

// Object that describes lower level features of the Play button
class PlayButton : public MIDI_Button
{
public:
    enum ButtonMode {
        SINGLE_PRESS,
        toLONG_PRESS,
        LONG_PRESS,
        toSINGLE_PRESS
    };

    ButtonMode MODE = SINGLE_PRESS;

    PlayButton(uint8_t i2cAddr, uint8_t interruptPin, uint32_t dbTime, BUTTON_LED_STATE* longPressState,
        BUTTON_LED_STATE* singlePressState, uint16_t timeForLongPress = 100u)
        : MIDI_Button(i2cAddr, interruptPin, dbTime)
        {
            m_midiStateName = BUTTON_MIDI_STATE::Name::PLAY;
            setup_singlePressLedIndicator(singlePressState);
            setup_longPress(longPressState, timeForLongPress);
        };

    void LedStateMachine(ButtonMode mode) {
        switch (mode) {
            case SINGLE_PRESS:
                enableSinglePressIndicator(true);
                toggleLedOff();
                break;
            case toLONG_PRESS:
            case LONG_PRESS:
                enableSinglePressIndicator(true);
                toggleLongPressLedOn();
                break;
            case toSINGLE_PRESS:
                break;
        }
    }

    void stateMachine(PushButtonEvent* event) override {
        if (event != nullptr) {

            ButtonMode nextMode = MODE;
            switch (MODE) {
                case SINGLE_PRESS:
                    if (event->type == PushButtonEvent::Type::MultiPressComplete) {
                        vPostEvent(m_midiStateName, BUTTON_MIDI_STATE::Type::RegularPress, event->pressCount);
                    }
                    else if (event->type == PushButtonEvent::Type::LongPress) {
                        nextMode = toLONG_PRESS;
                        vPostEvent(m_midiStateName, BUTTON_MIDI_STATE::Type::LongPress, 1);
                    }
                    break;

                case toLONG_PRESS:
                    if (event->type == PushButtonEvent::Type::LongPressEnd)
                        nextMode = LONG_PRESS;
                    break;

                case LONG_PRESS:
                    if (event->type == PushButtonEvent::Type::LongPress) {
                        nextMode = toSINGLE_PRESS;
                    }
                    else if (event->type == PushButtonEvent::Type::MultiPressComplete) {
                        vPostEvent(m_midiStateName, BUTTON_MIDI_STATE::Type::RegularPress, event->pressCount);
                    }
                    else if (event->type == PushButtonEvent::Type::Released) {
                        toggleLongPressLedOn();
                    }
                    break;

                case toSINGLE_PRESS:
                    if (event->type == PushButtonEvent::Type::LongPressEnd)
                        nextMode = SINGLE_PRESS;
                    break;
            }

            if (nextMode != MODE) {
                MODE = nextMode;
                LedStateMachine(MODE);
            }
        }
    }
};
