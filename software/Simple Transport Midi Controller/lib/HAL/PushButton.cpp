#include "PushButton.h"
#include <rSerial.h>

PushButton::PushButton(const char* name, uint8_t pinAddr, uint32_t dbTime, bool isInterruptPin, uint8_t mcuInputMode)
    : _pinBtn(pinAddr), _dbTime(dbTime), _isInterruptPin(isInterruptPin), _buttonPinMode(mcuInputMode)
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
        } else {
            m_eventQueue = xQueueCreate(queueLength, sizeof(PushButtonEvent));
        }
    }

    if (!_isInterruptPin) 
    {
        pinMode(_pinBtn, _buttonPinMode);
    }
    

    m_multiPressTimer = xTimerCreate(
        "BtnMP",
        pdMS_TO_TICKS(m_multiPressTimeLimit),
        pdFALSE,
        this,
        multiPressTimerCallback);

    m_longPressTimer = xTimerCreate(
        "BtnLP",                             
        pdMS_TO_TICKS(m_longPressTimeMs),     // initial period
        pdFALSE,                              // one-shot (auto-reload false)
        this,                                 // timer ID → PushButton*
        longPressTimerCallback);

    xTaskCreatePinnedToCore(
        vButtonTask, this->taskName, _taskStackSize, this, PUSH_BUTTON_FREERTOS_PRIORITY, &hdl_buttonTask, 1);

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
        rSerial.print("Button ");
        rSerial.println(pressed ? "Pressed" : "Released");
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

void PushButton::longPressTimerCallback(TimerHandle_t timer)
{
    auto* instance = static_cast<PushButton*>(pvTimerGetTimerID(timer));
    instance->longPressDetectedBegin();
}

void PushButton::longPressDetectedBegin()
{
    if (!m_state || mPOLL_longPressEdgeFired || m_longPressTimeMs == 0) {
        return;
    }

    mPOLL_longPressEdgeFired = true;
    m_longPressActive = true;
    mPOLL_longPressEdge = true;
    postEvent(PushButtonEvent::Type::LongPress);
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
            if (!m_pressIsCounted) {
                m_pressCount++;
                m_pressIsCounted = true;
            }

            mPOLL_longPressEdgeFired = false;
            mPOLL_pressEdge = true;
            postEvent(PushButtonEvent::Type::Pressed);

            if (m_multiPressTimer != nullptr) {
                xTimerStop(m_multiPressTimer, 0);
            }
            if (m_longPressTimer != nullptr && m_longPressTimeMs > 0) {
                xTimerChangePeriod(m_longPressTimer, pdMS_TO_TICKS(m_longPressTimeMs), 0);
                xTimerStart(m_longPressTimer, 0);
            }   

        } else {
            m_pressIsCounted = false;
            mPOLL_releaseEdge = true;
            postEvent(PushButtonEvent::Type::Released);

            // Emit a regular Press event (Pressed + Released) - for non press edge applications
            if (!mPOLL_longPressEdgeFired) { postEvent(PushButtonEvent::Type::RegularPressComplete);}

            if (ms - m_lastChange >= m_multiPressTimeLimit && !mPOLL_longPressEdgeFired) {
                m_pressCount = 0;
                if (m_multiPressTimer != nullptr) {
                    xTimerStop(m_multiPressTimer, 0);
                }
                finalizeMultiPress(1);
            } else if (mPOLL_longPressEdgeFired) {
                // Long press consumed this press cycle — don't start multi-press timer
                m_pressCount = 0;
                if (m_multiPressTimer != nullptr) {
                    xTimerStop(m_multiPressTimer, 0);
                }
            } else if (m_pressCount > 0 && m_multiPressTimer != nullptr) {
                xTimerChangePeriod(
                    m_multiPressTimer,
                    pdMS_TO_TICKS(m_multiPressTimeLimit),
                    0);
                xTimerStart(m_multiPressTimer, 0);
            }

            if (m_longPressTimer != nullptr) {
                // NOTE: m_longPressTimer is configured as one-shot, so stops if longPressTime exceeded before press release
                xTimerStop(m_longPressTimer, 0);
            }

            if (m_longPressActive) {
                m_longPressActive = false;
                postEvent(PushButtonEvent::Type::LongPressEnd);
            }
        }
    }


    if (m_state && m_longPressTimeMs > 0 && !mPOLL_longPressEdgeFired) {
        if (ms - m_lastChange >= m_longPressTimeMs) {
            rSerial.println("ln148 PB: Long Press Active");
            mPOLL_longPressEdgeFired = true;
            m_longPressActive = true;
            mPOLL_longPressEdge = true;
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
    mPOLL_multiPressReady = true;
    portEXIT_CRITICAL(&m_stateMux);

    postEvent(PushButtonEvent::Type::MultiPressComplete, count);
}

void PushButton::postEvent(PushButtonEvent::Type type, uint8_t pressCount)
{
    if (type == PushButtonEvent::Type::Pressed)  {digitalLedWrite(HIGH);}
    else if (type == PushButtonEvent::Type::Released){digitalLedWrite(LOW);}
    
    if (m_delivery != PushButtonDelivery::Queue || m_eventQueue == nullptr) {
        return;
    }

    PushButtonEvent event{type, pressCount};
    xQueueSend(m_eventQueue, (void *)&event, 0);
}

void PushButton::postEventPublic(PushButtonEvent::Type type, uint8_t pressCount)
{
    switch (type) {
            case PushButtonEvent::Type::Pressed:
                mPOLL_pressEdge = true;
                postEvent(PushButtonEvent::Type::Pressed, pressCount);
                break;
            case PushButtonEvent::Type::Released:
                mPOLL_releaseEdge = true;
                postEvent(PushButtonEvent::Type::Released, pressCount);
                break;

            case PushButtonEvent::Type::MultiPressComplete:
                finalizeMultiPress(pressCount);
                break;

            default:
                break;
        }
}

bool PushButton::consumePressEdge()
{
    if (!mPOLL_pressEdge) {
        return false;
    }
    mPOLL_pressEdge = false;
    return true;
}

bool PushButton::consumeReleaseEdge()
{
    if (!mPOLL_releaseEdge) {
        return false;
    }
    mPOLL_releaseEdge = false;
    return true;
}

bool PushButton::consumeLongPressEdge()
{
    if (!mPOLL_longPressEdge) {
        return false;
    }
    mPOLL_longPressEdge = false;
    return true;
}

uint8_t PushButton::consumeMultiPress()
{
    uint8_t count = 0;
    portENTER_CRITICAL(&m_stateMux);
    if (mPOLL_multiPressReady) {
        count = m_latchedMultiPress;
        m_latchedMultiPress = 0;
        mPOLL_multiPressReady = false;
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

void PushButton::setLongPressTimer(uint32_t ms)
{
    m_longPressTimeMs = ms;

    // Disable any actively running Timers if there is zero long press time
    if (ms == 0 && m_longPressTimer != nullptr) {
        xTimerStop(m_longPressTimer, 0);
    }

    if (m_longPressTimer != nullptr && ms > 0) {
        xTimerChangePeriod(m_longPressTimer, pdMS_TO_TICKS(ms), 0);
    }

}

uint32_t PushButton::getLongPressTime() const
{
    return m_longPressTimeMs;
}


bool PushButton::isLongPressActive() const
{
    return m_longPressActive;
}

// --- ToggleSwitch ---

ToggleSwitch::ToggleSwitch(const char* name, uint8_t pin, bool initialState, uint32_t dbTime, uint8_t inputMode)
    : PushButton(name, pin, dbTime, false, inputMode), m_toggleState(initialState)
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
    if (consumePressEdge()) {
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
