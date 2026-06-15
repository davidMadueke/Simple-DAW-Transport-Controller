#pragma once

#include <Arduino.h>
#include <FunctionalInterrupt.h>
#include <functional>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/timers.h>

#ifndef PUSH_BUTTON_FREERTOS_PRIORITY
    #define PUSH_BUTTON_FREERTOS_PRIORITY 4
#endif
#ifndef PUSH_BUTTON_FREERTOS_QUEUE_LENGTH 
    #define PUSH_BUTTON_FREERTOS_QUEUE_LENGTH 8
#endif


using DigitalReadCallback = std::function<bool()>;
using LedWriteCallback = std::function<void(bool)>;
struct PushButtonEvent {
    enum class Type : uint8_t {
        Pressed,
        Released,
        RegularPressComplete,
        MultiPressComplete,
        LongPress,
        LongPressEnd
    } type;
    uint8_t pressCount;
};

enum class PushButtonDelivery {
    Polling,
    Queue
};

/**
 * @class PushButton
 * @brief GPIO push button with ISR + FreeRTOS task debouncing,
 *        multi-press and long-press detection.
 *
 * Call begin() once; state is updated only from the button task.
 * Use polling getters or an optional FreeRTOS event queue per instance.
 */
class PushButton {
public:
    PushButton(const char* name, uint8_t pinAddr, uint32_t dbTime, bool isInterruptPin = false);

    void begin(PushButtonDelivery delivery = PushButtonDelivery::Polling,
               QueueHandle_t eventQueue = nullptr,
               UBaseType_t queueLength = PUSH_BUTTON_FREERTOS_QUEUE_LENGTH);

    void setButtonMode(uint8_t mode) {_buttonPinMode = mode;}

    void vSetTaskStackSize(uint32_t size) { _taskStackSize = size;}
    static void vButtonTask(void* pvParameters);
    void ISR_PushButton();
    void processTaskLoop();

    bool consumePressEdge();
    bool consumeReleaseEdge();

    uint8_t consumeMultiPress();
    void setMultiPressTimer(uint32_t multiPressTimeLimit);

    void setLongPressTimer(uint32_t ms);
    uint32_t getLongPressTime() const;

    bool consumeLongPressEdge();
    bool isLongPressActive() const;
    
    

    void postEventPublic(PushButtonEvent::Type type, uint8_t pressCount = 0);

    QueueHandle_t getButtonEventQueueHandle() {return m_eventQueue;};
    PushButtonDelivery getPushButtonDelivery() {return m_delivery;};

protected:
        // A callback function that will be applied after the stable state has been found
    virtual void onStableStateApplied();

private:
    void applyStableState(bool pressed);
    void finalizeMultiPress(uint8_t count);
    void postEvent(PushButtonEvent::Type type, uint8_t pressCount = 0);
    static void multiPressTimerCallback(TimerHandle_t timer);
    static void longPressTimerCallback(TimerHandle_t timer);
    void longPressDetectedBegin();
    

    bool m_state = false;
    bool m_lastState = false;
    bool m_changed = false;
    uint32_t m_time = 0;
    uint32_t m_lastChange = 0;

    uint8_t m_pressCount = 0;
    bool m_pressIsCounted = false;

    uint32_t m_multiPressTimeLimit = 150;
    bool m_longSinglePressPending = false;
    
   
    uint32_t m_longPressTimeMs = 500; 

    bool m_longPressActive = false;
    bool mPOLL_longPressEdgeFired = false;
    bool mPOLL_longPressEdge = false;

    volatile uint8_t m_latchedMultiPress = 0;
    volatile bool mPOLL_multiPressReady = false;

    bool mPOLL_pressEdge = false;
    bool mPOLL_releaseEdge = false;

    PushButtonDelivery m_delivery = PushButtonDelivery::Polling;
    QueueHandle_t m_eventQueue = nullptr;

    uint32_t _taskStackSize = 2048;

    uint8_t _pinBtn;
    uint8_t _buttonPinMode; // Same as arduino input mode (can either be INPUT, INPUT_PULLUP or INPUT_PULLDOWN)
    uint32_t _dbTime;
    bool _isInterruptPin; // Checker to make sure that attached pin isn't a button (requiring pinMode())

    // Reserve a fixed-size buffer matching FreeRTOS max task name length (16 bytes)
    char taskName[16]; 

    TaskHandle_t hdl_buttonTask = nullptr;
    TimerHandle_t m_multiPressTimer = nullptr;
    TimerHandle_t m_longPressTimer = nullptr;
    portMUX_TYPE m_stateMux = portMUX_INITIALIZER_UNLOCKED;

    // Attaching Digital Read Callback functionality
    public:
    void attachDigitalReadCallback(DigitalReadCallback callback){
        m_digitalReadCallback = std::move(callback);
    };

    void attachLedWriteCallback(LedWriteCallback callback){
        m_ledWriteCallback = std::move(callback);
    };

    protected:
        bool readPressed() {
            if (m_digitalReadCallback) return m_digitalReadCallback();
            return digitalRead(_pinBtn) == LOW;
        }

        void digitalLedWrite(bool onOff){
            if (m_ledWriteCallback) return m_ledWriteCallback(onOff);
            else return;
        }
    private:
        DigitalReadCallback m_digitalReadCallback;
        LedWriteCallback m_ledWriteCallback;
};

/**
 * @class ToggleSwitch
 * @brief Push-on / push-off toggle; call begin() after construction.
 */
class ToggleSwitch : public PushButton {
public:
    ToggleSwitch(const char* name, uint8_t pin, bool initialState = false, uint32_t dbTime = 25);

    void begin(PushButtonDelivery delivery = PushButtonDelivery::Polling,
               QueueHandle_t eventQueue = nullptr,
               UBaseType_t queueLength = 4);

    bool toggleState() const { return m_toggleState; }
    bool consumeChanged();

protected:
    void onStableStateApplied() override;

private:
    bool m_toggleState;
    bool m_changed = false;
};
