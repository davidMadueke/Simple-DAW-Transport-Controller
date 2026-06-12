#include <Button.h>
#include "Arduino.h"
#include <BUTTON_MIDI_STATE.h>
#include <MIDI_Button.h>

class QuantButton : public MIDI_Button {
public:
    QuantButton(
        uint8_t i2cAddr, uint8_t interruptPin, uint32_t dbTime,
        BUTTON_LED_STATE* singlePressState
    ) : MIDI_Button(i2cAddr, interruptPin, dbTime)
    {
        m_midiStateName = BUTTON_MIDI_STATE::Name::QUANT;
        setup_singlePressLedIndicator(singlePressState);
    };

    void stateMachine(PushButtonEvent* event) override {
        if (event != nullptr) {
            if (event->type == PushButtonEvent::Type::MultiPressComplete) {
                vPostEvent(m_midiStateName, BUTTON_MIDI_STATE::Type::RegularPress, event->pressCount);
            }
        }
    }
};
