#include <freertos/queue.h>
#include <InputSource.h>
#include <BUTTON_MIDI_STATE.h>
#include <PushButton.h>
#include "constants.h"


#ifndef INPUT_COMPILER_BUTTON_CC_VALUE
    #define INPUT_COMPILER_BUTTON_CC_VALUE 127
#endif

void dispatchButtonEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    BUTTON_MIDI_STATE evt;
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
                midiCC = MIDI_CC_PLAY_BTN;
                break;
            case BUTTON_MIDI_STATE::Name::STOP:
                midiCC = MIDI_CC_STOP_BTN;
                break;
            case BUTTON_MIDI_STATE::Name::REC:
                midiCC = MIDI_CC_REC_BTN;
                break;
            case BUTTON_MIDI_STATE::Name::QUANT:
                midiCC = MIDI_CC_QUANT_BTN;
                break;
            case BUTTON_MIDI_STATE::Name::OVERDUB:
                midiCC = MIDI_CC_OVERDUB_BTN;
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

        DISPLAY_ACTION action = createButtonPressAction(packet);
        if (xQueueSend(midiQueue, (void *)&action, 0) != pdTRUE) 
        {
            #ifdef INPUT_DISPATCHER_DEBUG
                Serial.printf("Debug for %s, line 57 of InputCompiler.h. Queue Full", (char*) midiQueue);
            #endif
        }
    }
};

void dispatchTapTempoEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    PushButtonEvent evt;
    if (xQueueReceive(inputQueue, &evt, 0) != pdTRUE) return;
    // map tap-tempo edge -> MIDI_CC_TAP_TEMPO etc.
    if (evt.type == PushButtonEvent::Type::RegularPressComplete) 
    {
        MIDI_PACKET packet{MIDI_PACKET::TYPE::CC, MIDI_CC_TAP_TEMPO, INPUT_COMPILER_BUTTON_CC_VALUE, MIDI_CHANNEL};
        if (xQueueSend(midiQueue, (void *)&packet, 0) != pdTRUE) 
        {
            #ifdef INPUT_DISPATCHER_DEBUG
                Serial.printf("Debug for %s, line 18 of InputCompiler.h. Queue Full", (char*) midiQueue);
            #endif
        }

        DISPLAY_ACTION action = createTapTempoAction(packet);
        if (xQueueSend(midiQueue, (void *)&action, 0) != pdTRUE) 
        {
            #ifdef INPUT_DISPATCHER_DEBUG
                Serial.printf("Debug for %s, line 57 of InputCompiler.h. Queue Full", (char*) midiQueue);
            #endif
        }
    }
};

void dispatchInfScrollEncoderEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    // Use the Arduino inbuilt constrain function to ensure that the new encoder value
    // is between 0 and 127
    //m_encoderValue = constrain(m_encoderValue, 0, 127);
};

void dispatchVolumeEncoderEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    // Use the Arduino inbuilt constrain function to ensure that the new encoder value
    // is between 0 and 127
    //m_encoderValue = constrain(m_encoderValue, 0, 127);
};

void inputDispatcher(InputSource input, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
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
            // dispatchEncoderEvent(input.queue, midiQueue, displayQueue);
            break;
        default:
            return;
    }
}