#include <freertos/queue.h>
#include <BUTTON_MIDI_STATE.h>
#include <PushButton.h>

void dispatchButtonEvent(QueueHandle_t queue)
{
    BUTTON_MIDI_STATE evt;
    if (xQueueReceive(queue, &evt, 0) != pdTRUE) return;
    // map evt.name / evt.type / evt.pressCount -> MIDI_PACKET + display action,
    // then xQueueSend to your midi-compile queue / display action queue
};

void dispatchTapTempoEvent(QueueHandle_t queue)
{
    PushButtonEvent evt;
    if (xQueueReceive(queue, &evt, 0) != pdTRUE) return;
    // map tap-tempo edge -> MIDI_CC_TAP_TEMPO etc.
};

void dispatchEncoderEvent(QueueHandle_t queue)
{
    
};