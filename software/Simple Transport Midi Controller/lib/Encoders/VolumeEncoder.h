#pragma once
#include "Arduino.h"
#include <HAL_RotaryEncoder.h>
#include <LedRing_DUPPA.h>
#include <DISPLAY_STATE.h>
#include <VOL_ENCODER_MIDI_STATE.h>
#include <rSerial.h>

#ifndef VOL_ENCODER_FREERTOS_PRIORITY
    #define VOL_ENCODER_FREERTOS_PRIORITY 3
#endif

#ifndef VOL_ENCODER_FREERTOS_TASK_STACK_SIZE
    #define VOL_ENCODER_FREERTOS_TASK_STACK_SIZE 4096
#endif

#ifndef VOL_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH
    #define VOL_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH 8
#endif

#ifndef VOL_ENCODER_QUEUE_RECEIVE_TIMEOUT_MS
    #define VOL_ENCODER_QUEUE_RECEIVE_TIMEOUT_MS 200
#endif

// Named RGB colour presets, expanded as (r, g, b) argument lists so they can be
// passed straight into setLedState(...). The per-mode assignment macros in
// constants.h reference these presets.
#ifndef VOL_ENCODER_LED_RING_BRIGHTNESS_PRESCALER
    #define VOL_ENCODER_LED_RING_BRIGHTNESS_PRESCALER 1U
#endif
static_assert(VOL_ENCODER_LED_RING_BRIGHTNESS_PRESCALER <= 255);

#define VOL_ENCODER_LED_RING_MAX_VALUE 255 / VOL_ENCODER_LED_RING_BRIGHTNESS_PRESCALER
#define VOL_ENCODER_LED_RING_OFF     0,   0,   0
#define VOL_ENCODER_LED_RING_PRESET_RED     VOL_ENCODER_LED_RING_MAX_VALUE, 0,   0
#define VOL_ENCODER_LED_RING_PRESET_GREEN   0,   VOL_ENCODER_LED_RING_MAX_VALUE, 0
#define VOL_ENCODER_LED_RING_PRESET_BLUE    0,   0,   VOL_ENCODER_LED_RING_MAX_VALUE
#define VOL_ENCODER_LED_RING_PRESET_TEST    128,   36,   99
#define VOL_ENCODER_LED_RING_PRESET_WHITE   VOL_ENCODER_LED_RING_MAX_VALUE, VOL_ENCODER_LED_RING_MAX_VALUE, VOL_ENCODER_LED_RING_MAX_VALUE



class VolumeEncoder {
    private:

    HAL_RotaryEncoder *encoder;
    LedRing* ring;

    // FreeRTOS task + output queue (consumed by InputManager)
    QueueHandle_t m_volEncoderQueue = nullptr;
    TaskHandle_t hdl_volEncoderTask = nullptr;

    VOL_ENCODER_MODE m_mode = (VOL_ENCODER_MODE) 1U; // Cast second MODE (i.e. First mode that is not NONE)
    uint8_t m_position = 0;

    // Injected pointers to the shared global display state and its mutex.
    // Attached via attachDisplayState(); used to re-sync the mode while idle.
    DISPLAY_STATE* m_displayState = nullptr;
    portMUX_TYPE* m_displayStateMux = nullptr;

    

    public:
    VolumeEncoder(
        uint8_t pinEnc1, uint8_t pinEnc2, uint8_t pinBtn, uint8_t dbTime,
        uint8_t i2cAddrLedRing, uint8_t pinBtnInputMode = INPUT_PULLUP
    ) 
     
    {
        encoder = new HAL_RotaryEncoder(pinEnc1, pinEnc2, pinBtn, dbTime, pinBtnInputMode);
        ring = new LedRing(i2cAddrLedRing);

    };

     // Inject pointers to the globally shared display state and its critical-section
    // mutex. Call before begin() so the initial mode/LEDs can be seeded.
    void attachDisplayState(DISPLAY_STATE* state, portMUX_TYPE* mux)
    {
        m_displayState = state;
        m_displayStateMux = mux;
    };

    // Sets up the Encoder and LED Ring tasks, along with a seperate tasks handling the logic between these elements
    // NOTE: ledRingStartValue must be a value between 0 and TOTAL_LEDS within LedRing Object
    void begin(uint8_t ledRingStartValue)
    {
        encoder->begin();
        encoder->disableLongPresses();
        ring->setLedStartValue(ledRingStartValue);
        #ifdef VOL_ENCODER_DEBUG
            rSerial.println("Volume Encoder began correctly");
        #endif
        ring->begin_dial();

        
        

        // Output queue of mode/encoder events for the InputManager to consume.
        m_volEncoderQueue = xQueueCreate(
            VOL_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH,
            sizeof(VOL_ENCODER_MIDI_STATE)
        );


        xTaskCreatePinnedToCore(
            vVolEncoderTask,
            "vol_encoder",
            VOL_ENCODER_FREERTOS_TASK_STACK_SIZE,
            this,
            VOL_ENCODER_FREERTOS_PRIORITY,
            &hdl_volEncoderTask,
            1
        );
    }

    static void vVolEncoderTask(void *pvParameters)
    {
        static_cast<VolumeEncoder*>(pvParameters)->processTaskLoop();
    }

    void processTaskLoop()
    {
        QueueHandle_t encQ = encoder->getRotaryEncoderEventQueueHandle();
        RotaryEncoderEvent ev{};

        while (true)
        {
            LedStateMachine(m_mode, m_position);

            if (m_displayState != nullptr && m_displayStateMux != nullptr)
            {
                // Idle: re-sync from the shared DISPLAY_STATE. If the mode was
                // changed elsewhere (e.g. a DisplayManager reducer), repaint the
                // LEDs but do NOT enqueue anything to the InputManager.
                VOL_ENCODER_MODE globalMode;
                uint8_t globalPosition = 0;

                portENTER_CRITICAL(m_displayStateMux);
                globalMode = m_displayState->volEncoderMode;
                globalPosition = m_displayState->volEncoder_potValue;
                portEXIT_CRITICAL(m_displayStateMux);

                if (globalMode != m_mode || m_position != globalPosition)
                {
                    m_mode = globalMode;
                    m_position = globalPosition;
                    m_position = constrain(m_position, 0, 127); // Guard to ensure position stays in MIDI number space
                    LedStateMachine(m_mode, m_position);
                }

            };
            
            
            if (xQueueReceive(encQ, &ev, pdMS_TO_TICKS(VOL_ENCODER_QUEUE_RECEIVE_TIMEOUT_MS)) == pdTRUE)
            {
                if (ev.type == RotaryEncoderEvent::Type::Button)
                {
                    // A completed regular press increments the mode, repaints the
                    // LEDs, and forwards the new mode to the InputManager.
                    if (ev.btnEvent.type == PushButtonEvent::Type::RegularPressComplete)
                    {
                        incrementMode();
                        LedStateMachine(m_mode, m_position);

                        VOL_ENCODER_MIDI_STATE state{m_mode, ev};
                        xQueueSend(m_volEncoderQueue, (void *)&state, 0);
                    }
                }
                else if (ev.type == RotaryEncoderEvent::Type::Encoder)
                {
                    // Delta is extracted and added to current m_Position, then forwarded thru
                    // NOTE: We are constraining position to MIDI space here before forwarding to downstream tasks
                    if(m_mode != VOL_ENCODER_MODE_NONE)
                    {
                        int8_t delta = ev.delta;
                        int16_t nextPosition = (int16_t)m_position + (int16_t)delta;
                        m_position = (uint8_t)constrain(nextPosition, 0, 127);
                        
                        #ifdef VOL_ENCODER_DEBUG
                            rSerial.print("  Pos= ");
                            rSerial.println(m_position);

            
                            rSerial.print("  delta= ");
                            rSerial.println(delta);
                        #endif
                    }
                    LedStateMachine(m_mode, m_position);

                    RotaryEncoderEvent evNewPosition {RotaryEncoderEvent::Type::Encoder, ev.btnEvent, m_position, ev.delta };
                    VOL_ENCODER_MIDI_STATE state{m_mode, evNewPosition};
                    xQueueSend(m_volEncoderQueue, (void *)&state, 0);
                }
            }
            
        }
    }; 

    void incrementMode()
    {
        m_mode = (VOL_ENCODER_MODE)((m_mode + 1) % VOL_ENCODER_MODE_COUNT);
        #ifdef VOL_ENCODER_DEBUG
            rSerial.println("Next Increment");
        #endif
    }


    void LedStateMachine(VOL_ENCODER_MODE mode, uint8_t position){
        LED_RING_DIAL_STATE state{};
        state.POSITION = ring->dial_midiToPosition(position); // Constrain whatever absolute Encoder Position is to LED Ring Dial Co-ordinates
        switch (mode)
        {
            case VOL_ENCODER_MODE::VOL_ENCODER_MODE_NONE:
                state.LED = {VOL_ENCODER_LED_RING_OFF};
                break;

            #define X(name, led, cc) case name: state.LED = {VOL_ENCODER_LED_RING_##led}; break;
                VOL_ENCODER_MODE_TABLE(X)
            #undef X

            default:
                state.LED = {VOL_ENCODER_LED_RING_OFF};
                break;
        }
        ring->overwriteLedRingDialState(state);
    };
};