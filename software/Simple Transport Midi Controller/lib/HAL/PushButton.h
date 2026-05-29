#pragma once

#include <Arduino.h>
#include <FunctionalInterrupt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/timers.h>

struct PushButtonEvent {
    enum class Type : uint8_t {
        Pressed,
        Released,
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
    PushButton(const char* name, uint8_t pinAddr, uint32_t dbTime);

    void begin(PushButtonDelivery delivery = PushButtonDelivery::Polling,
               QueueHandle_t eventQueue = nullptr,
               UBaseType_t queueLength = 4);

    static void vButtonTask(void* pvParameters);
    void IRAM_ATTR ISR_PushButton();
    void processTaskLoop();

    bool isPressed();
    bool isReleased();
    bool wasPressed();
    bool wasReleased();
    bool pressedFor(uint32_t ms);
    bool releasedFor(uint32_t ms);
    uint32_t lastChange();

    bool consumePressEdge();
    bool consumeReleaseEdge();

    uint8_t getAndClearMultiPress();
    void setMultiPressTimer(uint32_t multiPressTimeLimit);

    void setLongPressTime(uint32_t ms);
    uint32_t getLongPressTime() const;

    bool consumeLongPressEdge();
    bool isLongPressActive() const;

protected:
        // A callback function that will be applied after the stable state has been found
    virtual void onStableStateApplied();

private:
    void applyStableState(bool pressed);
    void finalizeMultiPress(uint8_t count);
    void postEvent(PushButtonEvent::Type type, uint8_t pressCount = 0);
    static void multiPressTimerCallback(TimerHandle_t timer);

    bool m_state = false;
    bool m_lastState = false;
    bool m_changed = false;
    uint32_t m_time = 0;
    uint32_t m_lastChange = 0;

    uint8_t m_pressCount = 0;
    bool m_pressRead = false;

    uint32_t m_multiPressTimeLimit = 150;
    bool m_longSinglePressPending = false;

    uint32_t m_longPressTimeMs = 1000;
    bool m_longPressActive = false;
    bool m_longPressEdgeFired = false;
    bool m_longPressEdge = false;

    volatile uint8_t m_latchedMultiPress = 0;
    volatile bool m_multiPressReady = false;

    bool m_pressEdge = false;
    bool m_releaseEdge = false;

    PushButtonDelivery m_delivery = PushButtonDelivery::Polling;
    QueueHandle_t m_eventQueue = nullptr;
    bool m_queueOwned = false;

    uint8_t _pinBtn;
    uint32_t _dbTime;

    // Reserve a fixed-size buffer matching FreeRTOS max task name length (16 bytes)
    char taskName[16]; 

    TaskHandle_t hdl_buttonTask = nullptr;
    TimerHandle_t m_multiPressTimer = nullptr;
    portMUX_TYPE m_stateMux = portMUX_INITIALIZER_UNLOCKED;
};

/**
 * @class ToggleSwitch
 * @brief Push-on / push-off toggle; call begin() after construction.
 */
class ToggleSwitch : public PushButton {
public:
    ToggleSwitch(uint8_t pin, bool initialState = false, uint32_t dbTime = 25);

    void begin(PushButtonDelivery delivery = PushButtonDelivery::Polling,
               QueueHandle_t eventQueue = nullptr,
               UBaseType_t queueLength = 4);

    bool toggleState() const { return m_toggleState; }
    bool consumeChanged();

protected:
    void onStableStateApplied() override;

private:
    bool m_toggleState = false;
    bool m_changed = false;
};
