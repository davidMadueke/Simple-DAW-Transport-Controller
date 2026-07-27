#include <freertos/queue.h>
#include <InputSource.h>
#include <BUTTON_MIDI_STATE.h>
#include <VOL_ENCODER_MIDI_STATE.h>
#include <PushButton.h>
#include "constants.h"
#include <rSerial.h>


#ifndef INPUT_COMPILER_BUTTON_CC_VALUE
    #define INPUT_COMPILER_BUTTON_CC_VALUE 127
#endif

inline void dispatchButtonEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    BUTTON_MIDI_STATE evt;
    DISPLAY_ACTION action;
    if (xQueueReceive(inputQueue, &evt, 0) != pdTRUE) return;
    // map evt.name / evt.type / evt.pressCount -> MIDI_PACKET + display action,
    // then xQueueSend to your midi-compile queue / display action queue

    // NOTE: All of the Buttons send discrete midi messages per regular button press
    // It suffices to figure out only the evt.name and evt.type
    if (evt.type == BUTTON_MIDI_STATE::Type::RegularPress)
    {
        uint8_t midiCC = 0;
        switch (evt.name)
        {
            case BUTTON_MIDI_STATE::Name::PLAY:
                midiCC = MIDI_CC_DAW_PLAY;
                break;
            case BUTTON_MIDI_STATE::Name::STOP:
                midiCC = MIDI_CC_DAW_STOP;
                break;
            case BUTTON_MIDI_STATE::Name::REC:
                midiCC = MIDI_CC_DAW_REC;
                break;
            case BUTTON_MIDI_STATE::Name::QUANT:
                midiCC = MIDI_CC_DAW_QUANT;
                break;
            case BUTTON_MIDI_STATE::Name::OVERDUB:
                midiCC = MIDI_CC_DAW_OVERDUB;
                break;
            case BUTTON_MIDI_STATE::Name::METRONOME:
                midiCC = MIDI_CC_DAW_METRONOME;
                break;
            
            default:
                return; // Should not reach this state, hence skip adding to the queues
        }

        MIDI_PACKET packet{MIDI_PACKET::TYPE::CC, midiCC, INPUT_COMPILER_BUTTON_CC_VALUE, MIDI_CHANNEL};
        if (xQueueSend(midiQueue, (void *)&packet, 0) != pdTRUE) 
        {
            #ifdef INPUT_DISPATCHER_DEBUG
                Serial.printf("Debug for %s, line 49 of InputCompiler.h. Queue Full", (char*) midiQueue);
            #endif
        }

        action = createButtonPressAction(packet);
        if (xQueueSend(displayQueue, (void *)&action, 0) != pdTRUE) 
        {
            #ifdef INPUT_DISPATCHER_DEBUG
                Serial.printf("Debug for %s, line 57 of InputCompiler.h. Queue Full", (char*) midiQueue);
            #endif
        }
    }
};

inline void dispatchTapTempoEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    PushButtonEvent evt;
    DISPLAY_ACTION action;
    if (xQueueReceive(inputQueue, &evt, 0) != pdTRUE) return;
    // map tap-tempo edge -> MIDI_CC_TAP_TEMPO etc.
    if (evt.type == PushButtonEvent::Type::RegularPressComplete) 
    {
        MIDI_PACKET packet{MIDI_PACKET::TYPE::CC, MIDI_CC_DAW_TAP_TEMPO, INPUT_COMPILER_BUTTON_CC_VALUE, MIDI_CHANNEL};
        if (xQueueSend(midiQueue, (void *)&packet, 0) != pdTRUE) 
        {
            #ifdef INPUT_DISPATCHER_DEBUG
                Serial.printf("Debug for %s, line 18 of InputCompiler.h. Queue Full", (char*) midiQueue);
            #endif
        }

        action = createTapTempoAction(packet);
        if (xQueueSend(displayQueue, (void *)&action, 0) != pdTRUE) 
        {
            #ifdef INPUT_DISPATCHER_DEBUG
                Serial.printf("Debug for %s, line 57 of InputCompiler.h. Queue Full", (char*) midiQueue);
            #endif
        }
    }
};

inline void dispatchInfScrollEncoderEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    // Use the Arduino inbuilt constrain function to ensure that the new encoder value
    // is between 0 and 127
    //m_encoderValue = constrain(m_encoderValue, 0, 127);
};

inline void dispatchVolumeEncoderEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    VOL_ENCODER_MIDI_STATE state;
    DISPLAY_ACTION action;

    if (xQueueReceive(inputQueue, &state, 0) != pdTRUE) return;

    // derive CC value from encoder mode
    uint8_t CC = MIDI_CC_INVALID;
    uint8_t pos = 0;
    switch (state.MODE)
        {
            case VOL_ENCODER_MODE::VOL_ENCODER_MODE_NONE:
                CC = MIDI_CC_INVALID;
                break;

            #define X(name, led, cc) case name: CC = cc; break;
                VOL_ENCODER_MODE_TABLE(X)
            #undef X

            default:
                CC = MIDI_CC_INVALID;
                break;
    }

    // map encoder turn -> MIDI_CC_TAP_TEMPO etc.
    if (state.event.type == RotaryEncoderEvent::Type::Button)
    {
        // We assume VOL_Encoder mode button changes of state are handled in underlying VolEncoder task
        action = createVolEncoderModeAction(state.MODE);

        if (xQueueSend(displayQueue, (void *)&action, 0) != pdTRUE) 
        {
            #ifdef INPUT_DISPATCHER_DEBUG
                Serial.printf("Debug for %s, line 133 of InputCompiler.h. Queue Full", (char*) midiQueue);
            #endif
        }
    }
    else if (state.event.type == RotaryEncoderEvent::Type::Encoder)
    {
        // Constrain midi value and check that CC number is not invalid
        if (CC != MIDI_CC_INVALID)
        {
            pos = constrain(state.event.encValue, 0, 127);

            MIDI_PACKET packet{MIDI_PACKET::TYPE::CC, CC, pos, MIDI_CHANNEL};
            if (xQueueSend(midiQueue, (void *)&packet, 0) != pdTRUE) 
            {
                #ifdef INPUT_DISPATCHER_DEBUG
                    Serial.printf("Debug for %s, line 113 of InputCompiler.h. Queue Full", (char*) midiQueue);
                #endif
            }

            action = createVolEncoderPositionAction(packet);
        

        if (xQueueSend(displayQueue, (void *)&action, 0) != pdTRUE) 
        {
            #ifdef INPUT_DISPATCHER_DEBUG
                Serial.printf("Debug for %s, line 158 of InputCompiler.h. Queue Full", (char*) midiQueue);
            #endif
        }
        }
    }
        
   
};

inline void inputDispatcher(InputSource input, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    switch (input.type) 
    {
        case InputSource::Type::RgbButton:
            dispatchButtonEvent(input.queue, midiQueue, displayQueue);
            break;
        case InputSource::Type::TapTempo:
            dispatchTapTempoEvent(input.queue, midiQueue, displayQueue);
            break;
        case InputSource::Type::InfScrollEncoder:
            // dispatchEncoderEvent(input.queue, midiQueue, displayQueue);
            break;
        case InputSource::Type::VolEncoder:
            dispatchVolumeEncoderEvent(input.queue, midiQueue, displayQueue);
            break;
        default:
            return;
    }
}