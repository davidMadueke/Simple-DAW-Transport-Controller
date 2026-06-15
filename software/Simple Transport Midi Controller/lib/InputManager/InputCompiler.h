#include <freertos/queue.h>
#include <InputSource.h>
#include <BUTTON_MIDI_STATE.h>
#include <PushButton.h>

void dispatchButtonEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    BUTTON_MIDI_STATE evt;
    if (xQueueReceive(inputQueue, &evt, 0) != pdTRUE) return;
    // map evt.name / evt.type / evt.pressCount -> MIDI_PACKET + display action,
    // then xQueueSend to your midi-compile queue / display action queue
};

void dispatchTapTempoEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    PushButtonEvent evt;
    if (xQueueReceive(inputQueue, &evt, 0) != pdTRUE) return;
    // map tap-tempo edge -> MIDI_CC_TAP_TEMPO etc.
    if (evt.type == PushButtonEvent::Type::Pressed) {
        
    }
};

void dispatchEncoderEvent(QueueHandle_t inputQueue, QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    
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
        case InputSource::Type::Encoder:
            // dispatchEncoderEvent(input.queue, midiQueue, displayQueue);
            break;
    }
}