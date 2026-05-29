#include "PushButton.h"

PushButton::PushButton(const char* name, uint8_t pinAddr, uint32_t dbTime)
    : _pinBtn(pinAddr), _dbTime(dbTime)
{
    snprintf(this->taskName, sizeof(this->taskName), "%s_btn", name);
    m_time = millis();
    m_lastChange = m_time;
}

void PushButton::begin(PushButtonDelivery delivery,
                       QueueHandle_t eventQueue,
                       UBaseType_t queueLength)
{
    m_delivery = delivery;

    if (m_delivery == PushButtonDelivery::Queue) {
        if (eventQueue != nullptr) {
            m_eventQueue = eventQueue;
            m_queueOwned = false;
        } else {
            m_eventQueue = xQueueCreate(queueLength, sizeof(PushButtonEvent));
            m_queueOwned = (m_eventQueue != nullptr);
        }
    }

    pinMode(_pinBtn, INPUT_PULLUP);

    m_multiPressTimer = xTimerCreate(
        "BtnMP",
        pdMS_TO_TICKS(m_multiPressTimeLimit),
        pdFALSE,
        this,
        multiPressTimerCallback);

    xTaskCreatePinnedToCore(
        vButtonTask, this->taskName, 2048, this, 3, &hdl_buttonTask, 1);

    attachInterrupt(
        digitalPinToInterrupt(_pinBtn),
        std::bind(&PushButton::ISR_PushButton, this),
        CHANGE);
}

void PushButton::vButtonTask(void* pvParameters)
{
    static_cast<PushButton*>(pvParameters)->processTaskLoop();
}

void PushButton::processTaskLoop()
{
    const TickType_t debounceDelay = pdMS_TO_TICKS(_dbTime);

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ulTaskNotifyTake(pdTRUE, debounceDelay);

        bool pressed = readPressed();
        applyStableState(pressed);

#ifdef BUTTON_DEBUG
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, pressed);
        Serial.print("Button ");
        Serial.println(pressed ? "Pressed" : "Released");
#endif
    }
}

void IRAM_ATTR PushButton::ISR_PushButton()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (hdl_buttonTask != nullptr) {
        vTaskNotifyGiveFromISR(hdl_buttonTask, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void PushButton::multiPressTimerCallback(TimerHandle_t timer)
{
    auto* instance = static_cast<PushButton*>(pvTimerGetTimerID(timer));
    if (instance->m_pressCount > 0) {
        instance->finalizeMultiPress(instance->m_pressCount);
        instance->m_pressCount = 0;
    }
}

void PushButton::applyStableState(bool pressed)
{
    uint32_t ms = millis();
    m_lastState = m_state;
    m_state = pressed;
    m_changed = (m_state != m_lastState);
    m_time = ms;

    if (m_changed) {
        m_lastChange = ms;

        if (m_state) {
            if (!m_pressRead) {
                m_pressCount++;
                m_pressRead = true;
            }
            m_longPressEdgeFired = false;
            m_longSinglePressPending = false;
            m_pressEdge = true;
            postEvent(PushButtonEvent::Type::Pressed);

            if (m_multiPressTimer != nullptr) {
                xTimerStop(m_multiPressTimer, 0);
            }
        } else {
            m_pressRead = false;
            m_releaseEdge = true;
            postEvent(PushButtonEvent::Type::Released);

            if (m_longSinglePressPending) {
                m_longSinglePressPending = false;
                m_pressCount = 0;
                if (m_multiPressTimer != nullptr) {
                    xTimerStop(m_multiPressTimer, 0);
                }
                finalizeMultiPress(1);
            } else if (m_pressCount > 0 && m_multiPressTimer != nullptr) {
                xTimerChangePeriod(
                    m_multiPressTimer,
                    pdMS_TO_TICKS(m_multiPressTimeLimit),
                    0);
                xTimerStart(m_multiPressTimer, 0);
            }

            if (m_longPressActive) {
                m_longPressActive = false;
                postEvent(PushButtonEvent::Type::LongPressEnd);
            }
        }
    }

    if (m_state && ms - m_lastChange >= m_multiPressTimeLimit) {
        m_longSinglePressPending = true;
    }

    if (m_state && m_longPressTimeMs > 0 && !m_longPressEdgeFired) {
        if (ms - m_lastChange >= m_longPressTimeMs) {
            m_longPressEdgeFired = true;
            m_longPressActive = true;
            m_longPressEdge = true;
            postEvent(PushButtonEvent::Type::LongPress);
        }
    }

    onStableStateApplied();
}

void PushButton::onStableStateApplied() {}

void PushButton::finalizeMultiPress(uint8_t count)
{
    if (count == 0) {
        return;
    }

    portENTER_CRITICAL(&m_stateMux);
    m_latchedMultiPress = count;
    m_multiPressReady = true;
    portEXIT_CRITICAL(&m_stateMux);

    postEvent(PushButtonEvent::Type::MultiPressComplete, count);
}

void PushButton::postEvent(PushButtonEvent::Type type, uint8_t pressCount)
{
    if (m_delivery != PushButtonDelivery::Queue || m_eventQueue == nullptr) {
        return;
    }

    PushButtonEvent event{type, pressCount};
    xQueueSend(m_eventQueue, &event, 0);
}

bool PushButton::isPressed()
{
    return m_state;
}

bool PushButton::isReleased()
{
    return !m_state;
}

bool PushButton::wasPressed()
{
    return m_state && m_changed;
}

bool PushButton::wasReleased()
{
    return !m_state && m_changed;
}

bool PushButton::pressedFor(uint32_t ms)
{
    return m_state && (m_time - m_lastChange >= ms);
}

bool PushButton::releasedFor(uint32_t ms)
{
    return !m_state && (m_time - m_lastChange >= ms);
}

uint32_t PushButton::lastChange()
{
    return m_lastChange;
}

bool PushButton::consumePressEdge()
{
    if (!m_pressEdge) {
        return false;
    }
    m_pressEdge = false;
    return true;
}

bool PushButton::consumeReleaseEdge()
{
    if (!m_releaseEdge) {
        return false;
    }
    m_releaseEdge = false;
    return true;
}

uint8_t PushButton::getAndClearMultiPress()
{
    uint8_t count = 0;
    portENTER_CRITICAL(&m_stateMux);
    if (m_multiPressReady) {
        count = m_latchedMultiPress;
        m_latchedMultiPress = 0;
        m_multiPressReady = false;
    }
    portEXIT_CRITICAL(&m_stateMux);
    return count;
}

void PushButton::setMultiPressTimer(uint32_t multiPressTimeLimit)
{
    m_multiPressTimeLimit = multiPressTimeLimit;
    if (m_multiPressTimer != nullptr) {
        xTimerChangePeriod(
            m_multiPressTimer,
            pdMS_TO_TICKS(m_multiPressTimeLimit),
            0);
    }
}

void PushButton::setLongPressTime(uint32_t ms)
{
    m_longPressTimeMs = ms;
}

uint32_t PushButton::getLongPressTime() const
{
    return m_longPressTimeMs;
}

bool PushButton::consumeLongPressEdge()
{
    if (!m_longPressEdge) {
        return false;
    }
    m_longPressEdge = false;
    return true;
}

bool PushButton::isLongPressActive() const
{
    return m_longPressActive;
}

// --- ToggleSwitch ---

ToggleSwitch::ToggleSwitch(const char* name, uint8_t pin, bool initialState, uint32_t dbTime)
    : PushButton(name, pin, dbTime), m_toggleState(initialState)
{
}

void ToggleSwitch::begin(PushButtonDelivery delivery,
                         QueueHandle_t eventQueue,
                         UBaseType_t queueLength)
{
    PushButton::begin(delivery, eventQueue, queueLength);
}

void ToggleSwitch::onStableStateApplied()
{
    if (wasPressed()) {
        m_toggleState = !m_toggleState;
        m_changed = true;
    }
}

bool ToggleSwitch::consumeChanged()
{
    if (!m_changed) {
        return false;
    }
    m_changed = false;
    return true;
}
