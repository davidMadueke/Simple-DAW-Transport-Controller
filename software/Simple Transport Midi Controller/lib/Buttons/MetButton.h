#include <Button.h>
#include "Arduino.h"
#include <BUTTON_MIDI_STATE.h>
#include <Jack_TapTempo.h>
#include <MIDI_Button.h>

// Object that describes lower level features of the Metronome (Tap Tempo) button
class MetButton : public MIDI_Button
{
    private:
        Jack_TapTempo* _TapTempo;
public:
    enum ButtonMode {
        METRONOME_OFF,
        METRONOME_ON,
        toLONG_PRESS,
        LONG_PRESS,
        toCURR_METRONOME_STATE
    };
    
    ButtonMode MODE = METRONOME_OFF;
    ButtonMode METRONOME_TOGGLE_STATE = METRONOME_OFF; // To preserve the state that metronome will be in

    MetButton(uint8_t i2cAddr, uint8_t interruptPin, uint32_t dbTime, BUTTON_LED_STATE* longPressState, 
        BUTTON_LED_STATE* singlePressState, BUTTON_LED_STATE* toggleLedState, Jack_TapTempo* TapTempo,
        uint16_t timeForLongPress = 500u)
        : MIDI_Button(i2cAddr, interruptPin, dbTime), _TapTempo(TapTempo)
        {
            m_midiStateName = BUTTON_MIDI_STATE::Name::METRONOME;
            setup_singlePressLedIndicator(singlePressState);
            setup_longPress(longPressState, timeForLongPress);
            setup_toggleLedState(toggleLedState);
        };
    
    // LED state machine
    void LedStateMachine(ButtonMode mode) {
    switch (mode) {
        case METRONOME_OFF:
            enableSinglePressIndicator(false);   // solid, no per-press flash
            toggleLedOff();
            break;
        case METRONOME_ON:
            enableSinglePressIndicator(false);
            toggleLedOn();
            break;
        case toLONG_PRESS:
        case LONG_PRESS:
            enableSinglePressIndicator(true);
            toggleLongPressLedOn();
            break;                      // tap-tempo: want per-press feedback
        case toCURR_METRONOME_STATE:
            break;
    }
}

    // Override stateMachine
    void stateMachine(PushButtonEvent* event) override {
        if (event != nullptr){

            ButtonMode nextMode = MODE;
            switch (MODE) {
                case METRONOME_OFF:
                    METRONOME_TOGGLE_STATE = METRONOME_OFF;
                    if (event->type == PushButtonEvent::Type::LongPress) {
                        nextMode = toLONG_PRESS;
                        //vPostEvent(BUTTON_MIDI_STATE::Type::RegularPress, 1);
                        Serial.println("Going to Long Press");
                        break;
                    }
                    else if (event->type == PushButtonEvent::Type::RegularPressComplete){
                            nextMode = METRONOME_ON;
                            METRONOME_TOGGLE_STATE = METRONOME_ON;
                            vPostEvent(m_midiStateName,BUTTON_MIDI_STATE::Type::RegularPress, 1);
                            Serial.println("Turning On");
                            break;
                        }
                    else {
                        break;
                    }
                
                case METRONOME_ON:
                    if (event->type == PushButtonEvent::Type::RegularPressComplete){
                            vPostEvent(m_midiStateName, BUTTON_MIDI_STATE::Type::RegularPress, 1);
                            nextMode = METRONOME_OFF;
                            Serial.println("Turning Off");
                        }
                        
                    if (event->type == PushButtonEvent::Type::LongPress) {
                        nextMode = toLONG_PRESS;
                        Serial.println("Going to Long Press");
                        //vPostEvent(BUTTON_MIDI_STATE::Type::RegularPress, 1);
                        break;
                    }
                    break;

                case toLONG_PRESS:
                    if (event->type == PushButtonEvent::Type::LongPressEnd)
                        nextMode = LONG_PRESS;
                        break;

                case LONG_PRESS:
                    if (event->type == PushButtonEvent::Type::LongPress) {
                        nextMode = toCURR_METRONOME_STATE;
                        if (_TapTempo != nullptr) { _TapTempo->addInducedTapTempoEventToQueue(PushButtonEvent::Type::Released); }
                        // Not registering a single press to TAP TEMPO Queue.
                        // Do not want to send a Tap Tempo Midi Message when switching back to Metronome Mode
                    }
                    else {
                        
                        // if (event->type == PushButtonEvent::Type::Released) {
                        //     Serial.println("TT Pressing");
                        // }

                        if (_TapTempo != nullptr && (event->type == PushButtonEvent::Type::Released || event->type == PushButtonEvent::Type::Pressed)) {
                            // When in Tap Tempo mode, any button presses are registered in the
                            // TapTempo Object Event Queue as a Press/Release
                            _TapTempo->addInducedTapTempoEventToQueue(event->type);
                        }
                    }
                    break;

                case toCURR_METRONOME_STATE:
                    if (event->type == PushButtonEvent::Type::LongPressEnd)
                        nextMode = METRONOME_TOGGLE_STATE;
                    break;
            }
        
            if (nextMode != MODE){
                MODE = nextMode;
                LedStateMachine(MODE);
            }

        }
    }
};
