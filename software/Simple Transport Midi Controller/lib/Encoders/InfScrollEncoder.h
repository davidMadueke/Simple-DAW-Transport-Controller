#pragma once
#include "Arduino.h"
#include <HAL_RotaryEncoder.h>
#include <INF_SCROLL_MIDI_STATE.h>
#include <DISPLAY_STATE.h>
#include <rSerial.h>
#include <functional>

#ifndef INF_SCROLL_ENCODER_FREERTOS_PRIORITY
    #define INF_SCROLL_ENCODER_FREERTOS_PRIORITY 3
#endif

#ifndef INF_SCROLL_ENCODER_FREERTOS_TASK_STACK_SIZE
    #define INF_SCROLL_ENCODER_FREERTOS_TASK_STACK_SIZE 4096
#endif

#ifndef INF_SCROLL_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH
    #define INF_SCROLL_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH 8
#endif

#ifndef INF_SCROLL_ENCODER_QUEUE_RECEIVE_TIMEOUT_MS
    #define INF_SCROLL_ENCODER_QUEUE_RECEIVE_TIMEOUT_MS 200
#endif

// Named RGB colour presets, expanded as (r, g, b) argument lists so they can be
// passed straight into setLedState(...). The per-mode assignment macros in
// constants.h reference these presets.
#ifndef INF_SCROLL_LED_BRIGHTNESS_PRESCALER
    #define INF_SCROLL_LED_BRIGHTNESS_PRESCALER 1U
#endif
static_assert(INF_SCROLL_LED_BRIGHTNESS_PRESCALER <= 255);

#define INF_SCROLL_LED_MAX_VALUE 255 / INF_SCROLL_LED_BRIGHTNESS_PRESCALER
#define INF_SCROLL_LED_OFF     0,   0,   0
#define INF_SCROLL_LED_PRESET_RED     INF_SCROLL_LED_MAX_VALUE, 0,   0
#define INF_SCROLL_LED_PRESET_GREEN   0,   INF_SCROLL_LED_MAX_VALUE, 0
#define INF_SCROLL_LED_PRESET_BLUE    0,   0,   INF_SCROLL_LED_MAX_VALUE
#define INF_SCROLL_LED_PRESET_TEST    128,   36,   99
#define INF_SCROLL_LED_PRESET_WHITE   INF_SCROLL_LED_MAX_VALUE, INF_SCROLL_LED_MAX_VALUE, INF_SCROLL_LED_MAX_VALUE

#include "constants.h"

using InfScrollLedWriteCallback = std::function<void(uint8_t, uint8_t)>;
using LedPinModeCallback = std::function<void(uint8_t, uint8_t)>;
struct INF_ENCODER_LED_STATE
{
    uint8_t Red;
    uint8_t Blue;
    uint8_t Green;
};
class InfScrollEncoder {
    private:
    HAL_RotaryEncoder *encoder;
    
    INF_ENCODER_LED_STATE m_ledState = {INF_SCROLL_LED_OFF};

    uint8_t m_ledPin_R; // Pin number for the red LED
    uint8_t m_ledPin_G; // Pin number for the green LED
    uint8_t m_ledPin_B; // Pin number for the blue LED

    bool m_ledWriteCallbackAttached = false;
    InfScrollLedWriteCallback m_ledWriteCallback;
    LedPinModeCallback m_ledPinModeCallback;

    // FreeRTOS task + output queue (consumed by InputManager)
    QueueHandle_t m_infScrollQueue = nullptr;
    TaskHandle_t hdl_infScrollTask = nullptr;

    INF_SCROLL_MODE m_mode = (INF_SCROLL_MODE)1U; // Cast second MODE (i.e. First mode that is not NONE)

    // Injected pointers to the shared global display state and its mutex.
    // Attached via attachDisplayState(); used to re-sync the mode while idle.
    DISPLAY_STATE* m_displayState = nullptr;
    portMUX_TYPE* m_displayStateMux = nullptr;

    void _pinMode(uint8_t pin, uint8_t mode)
    {
        if (m_ledPinModeCallback) return m_ledPinModeCallback(pin, mode);
        else return pinMode(pin, mode); // Default Arduino pin mode
    }

    public:
    InfScrollEncoder(
        uint8_t pinEnc1, uint8_t pinEnc2, uint8_t pinBtn, uint8_t dbTime, 
        uint8_t ledPin_R, uint8_t ledPin_G, uint8_t ledPin_B, uint8_t pinBtnInputMode = INPUT_PULLUP
    ) 
        : m_ledPin_R(ledPin_R), m_ledPin_G(ledPin_G), m_ledPin_B(ledPin_B) 
    {
        encoder = new HAL_RotaryEncoder(pinEnc1, pinEnc2, pinBtn, dbTime, pinBtnInputMode);
    };

    // Inject pointers to the globally shared display state and its critical-section
    // mutex. Call before begin() so the initial mode/LEDs can be seeded.
    void attachDisplayState(DISPLAY_STATE* state, portMUX_TYPE* mux)
    {
        m_displayState = state;
        m_displayStateMux = mux;
    };

    void begin() 
    {
        encoder->begin();
        encoder->disableLongPresses();
        ledSetup();
        // Output queue of mode/encoder events for the InputManager to consume.
        m_infScrollQueue = xQueueCreate(
            INF_SCROLL_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH,
            sizeof(INF_SCROLL_MIDI_STATE)
        );


        xTaskCreatePinnedToCore(
            vInfScrollTask,
            "inf_scroll",
            INF_SCROLL_ENCODER_FREERTOS_TASK_STACK_SIZE,
            this,
            INF_SCROLL_ENCODER_FREERTOS_PRIORITY,
            &hdl_infScrollTask,
            1
        );
    }

    static void vInfScrollTask(void *pvParameters)
    {
        static_cast<InfScrollEncoder*>(pvParameters)->processTaskLoop();
    }


    void incrementMode()
    {
        m_mode = (INF_SCROLL_MODE)((m_mode + 1) % INF_SCROLL_MODE_COUNT);
        #ifdef INF_SCROLL_DEBUG
            rSerial.println("Next Increment");
        #endif
    }

    void LedStateMachine(INF_SCROLL_MODE mode)
    {
        switch (mode)
        {
            #define X(name, led, cc) case name: setLedState(INF_SCROLL_LED_##led); break;
                INF_SCROLL_MODE_TABLE(X)
            #undef X

            default:
                setLedState(INF_SCROLL_LED_OFF);
                break;
        }
        setLeds();
    }

    QueueHandle_t getInfScrollEventQueueHandle() { return m_infScrollQueue; }

    void attachLedWriteCallback(InfScrollLedWriteCallback callback)
    {
        m_ledWriteCallback = std::move(callback);
    };

    void attachLedPinModeCallback(LedPinModeCallback callback)
    {
         m_ledPinModeCallback = std::move(callback);
    };


    void ledWrite(uint8_t pin, uint8_t value)
    {
        if (m_ledWriteCallback) return m_ledWriteCallback(pin, value);
        else return;
    }

    void ledSetup() 
    {
        _pinMode(m_ledPin_R, OUTPUT);
        _pinMode(m_ledPin_G, OUTPUT);
        _pinMode(m_ledPin_B, OUTPUT);

        // Seed the internal mode from the shared global state and paint the LEDs.
        if (m_displayState != nullptr && m_displayStateMux != nullptr)
        {
            portENTER_CRITICAL(m_displayStateMux);
            m_mode = m_displayState->infScrollMode;
            portEXIT_CRITICAL(m_displayStateMux);
        }

    };

    void setLedState(uint8_t red, uint8_t green, uint8_t blue) 
    {
        m_ledState.Red = red;
        m_ledState.Green = green;
        m_ledState.Blue = blue;

    };
        
    void setLeds()
    {
        ledWrite(m_ledPin_R, m_ledState.Red);
        ledWrite(m_ledPin_G, m_ledState.Green);
        ledWrite(m_ledPin_B, m_ledState.Blue);
    }

    void processTaskLoop()
    {
        QueueHandle_t encQ = encoder->getRotaryEncoderEventQueueHandle();
        RotaryEncoderEvent ev{};

        while (true)
        {
            LedStateMachine(m_mode);

            if (m_displayState != nullptr && m_displayStateMux != nullptr)
            {
                // Idle: re-sync from the shared DISPLAY_STATE. If the mode was
                // changed elsewhere (e.g. a DisplayManager reducer), repaint the
                // LEDs but do NOT enqueue anything to the InputManager.
                INF_SCROLL_MODE globalMode;
                portENTER_CRITICAL(m_displayStateMux);
                globalMode = m_displayState->infScrollMode;
                portEXIT_CRITICAL(m_displayStateMux);

                if (globalMode != m_mode)
                {
                    m_mode = globalMode;
                    LedStateMachine(m_mode);
                }
            };
            
            
            if (xQueueReceive(encQ, &ev, pdMS_TO_TICKS(INF_SCROLL_ENCODER_QUEUE_RECEIVE_TIMEOUT_MS)) == pdTRUE)
            {
                if (ev.type == RotaryEncoderEvent::Type::Button)
                {
                    // A completed regular press increments the mode, repaints the
                    // LEDs, and forwards the new mode to the InputManager.
                    if (ev.btnEvent.type == PushButtonEvent::Type::RegularPressComplete)
                    {
                        incrementMode();
                        LedStateMachine(m_mode);

                        INF_SCROLL_MIDI_STATE state{m_mode, ev};
                        xQueueSend(m_infScrollQueue, (void *)&state, 0);
                    }
                }
                else if (ev.type == RotaryEncoderEvent::Type::Encoder)
                {
                    // Rotation/delta events are forwarded as-is under the current
                    // mode. No LED change for rotation.
                    INF_SCROLL_MIDI_STATE state{m_mode, ev};
                    xQueueSend(m_infScrollQueue, (void *)&state, 0);
                }
            }
            
        }
    };
};
