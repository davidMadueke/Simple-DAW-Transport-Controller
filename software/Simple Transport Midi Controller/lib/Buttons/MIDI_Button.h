#pragma once
#include "Arduino.h"
#include <Button.h>
#include <BUTTON_MIDI_STATE.h>
#include <functional>


#ifndef MIDI_BUTTON_FREERTOS_PRIORITY
    #define MIDI_BUTTON_FREERTOS_PRIORITY 3
#endif

#ifndef MIDI_BUTTON_FREERTOS_TASK_STACK_SIZE
    #define MIDI_BUTTON_FREERTOS_TASK_STACK_SIZE 16384
#endif

#ifndef MIDI_BUTTON_FREERTOS_EVENT_QUEUE_LENGTH
    #define MIDI_BUTTON_FREERTOS_EVENT_QUEUE_LENGTH 8
#endif


// Define the base class MIDI_Button
class MIDI_Button {
protected:
    uint16_t m_LONG_PRESS_ms;

    // Setup stored LED States struct for different types of buttons to be setup
    BUTTON_LED_STATE* m_longPressLedState = nullptr;
    BUTTON_LED_STATE* m_singlePressLedIndicator = nullptr;
    BUTTON_LED_STATE* m_toggleLedState = nullptr;
    bool m_noLedIndicator;
    
    Button* button;
    PushButtonEvent event;
    BUTTON_MIDI_STATE::Name m_midiStateName;
    QueueHandle_t m_eventQueue = nullptr;

public:
    
    MIDI_Button( uint8_t i2cAddr, uint8_t interruptPin, uint32_t dbTime) {
        button = new Button("midi_push", i2cAddr, interruptPin, dbTime);
        m_noLedIndicator = true;
        
    }

    void begin(uint8_t brightnessPrescaler,
        uint8_t multiPressTimerMs = 150u, uint16_t longPressTimerMs = 500u) {
        button->begin(PushButtonDelivery::Queue, nullptr, MIDI_BUTTON_FREERTOS_EVENT_QUEUE_LENGTH);
        setBrightness(brightnessPrescaler);
        button->setMultiPressTimer(multiPressTimerMs);
        button->setLongPressTimer(longPressTimerMs);
        m_eventQueue = xQueueCreate(
            MIDI_BUTTON_FREERTOS_EVENT_QUEUE_LENGTH, 
            sizeof(BUTTON_MIDI_STATE)
        );

        xTaskCreatePinnedToCore(
            vButtonMidiTask, 
            "midi_btn", 
            (uint32_t)MIDI_BUTTON_FREERTOS_TASK_STACK_SIZE, 
            this, 
            MIDI_BUTTON_FREERTOS_PRIORITY, 
            &hdl_buttonMidiTask, 
            1
        );

    }

    static void vButtonMidiTask(void* pvParameters){
        static_cast<MIDI_Button*>(pvParameters)->stateMachineLoop();
    };

    QueueHandle_t getEventQueueHandle() { return m_eventQueue;}

private:
    TaskHandle_t hdl_buttonMidiTask = nullptr;

    void stateMachineLoop(){
        
        while (true) {
            if (xQueueReceive(button->getButtonEventQueueHandle(), (void *)&event, 0) == pdTRUE) {
                #ifdef MIDI_BUTTON_DEBUG
                    Serial.printf("Debug for %s, line 77 of Midi Button. Queue Empty", taskName);
                #endif
                stateMachine(&event);
            }
            
        }
    };

protected:
    virtual ~MIDI_Button() {}

    // Setup an SinglePressLedIndicator begin
    virtual void setup_singlePressLedIndicator(BUTTON_LED_STATE* singlePressState){
        m_noLedIndicator = false;
        m_singlePressLedIndicator = singlePressState;
        setSinglePressLedIndicator(
            m_singlePressLedIndicator->Red,
            m_singlePressLedIndicator->Green,
            m_singlePressLedIndicator->Blue
        );

    }

    // Setup a LongPressLedState begin
    virtual void setup_longPress(BUTTON_LED_STATE* longPressState, uint16_t timeForLongPress = 1000u ){
        setTimeForLongPress(timeForLongPress);
        m_longPressLedState = longPressState;
    }

    virtual void setup_toggleLedState(BUTTON_LED_STATE* toggleLedState){
        m_toggleLedState = toggleLedState;
    }

    virtual void setBrightness(uint8_t brightnessPrescaler) { button->setBrightness(brightnessPrescaler); }

    virtual void setLedColour(uint8_t r, uint8_t g, uint8_t b) { button->setLedColour(r, g, b); }

    virtual void toggleLedOn() {
        setLedColour(m_toggleLedState);
    }

    virtual void toggleLedOff(){ button->ledOff(); }

    virtual void toggleLongPressLedOn(){ setLedColour(m_longPressLedState);};

    virtual void setLedColour(BUTTON_LED_STATE* state) {
        setLedColour(
            state->Red,
            state->Green,
            state->Blue
        );
    }

    virtual void setSinglePressLedIndicator(uint8_t r, uint8_t g, uint8_t b) { 
        if(!m_noLedIndicator){
            button->setSinglePressLedIndicator(r, g, b); 
        }
    }

    virtual void enableSinglePressIndicator(bool on_off) {
    if (!m_noLedIndicator) {
        button->enableSinglePressIndicator(on_off);
        }
    }

    virtual void setTimeForLongPress(uint8_t time_ms) { 
        m_LONG_PRESS_ms = time_ms; 
        button->setLongPressTimer(time_ms);
    };

    virtual uint8_t getTimeForLongPress(){ return m_LONG_PRESS_ms; };

    void vPostEvent(BUTTON_MIDI_STATE::Name name, BUTTON_MIDI_STATE::Type type, uint8_t pressCount = 0){
        BUTTON_MIDI_STATE event{name, type, pressCount};
        
        if (xQueueSend(m_eventQueue, (void *)&event, 0) != pdTRUE) {
                #ifdef MIDI_BUTTON_DEBUG
                    Serial.printf("Debug for %s, line 145 of Midi Button. Queue Full", taskName);
                #endif
        }
        return;
    };

    // Virtual stateMachine to be overridden
    virtual void stateMachine(PushButtonEvent* event) = 0;
};