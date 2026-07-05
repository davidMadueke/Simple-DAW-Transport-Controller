#pragma once
#include "Arduino.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

// ---------------------------------------------------------------------------
// rSerial - asynchronous serial logger.
//
// Inherits Arduino's Print, so print()/println()/printf()/typed overloads all
// work like Serial. Each completed line is buffered and pushed into a FreeRTOS
// queue; a dedicated task created in begin() drains the queue and is the only
// context that touches the underlying port.
//
// Caveat: a single println() emits two write() calls (content + '\n'). Under
// heavy concurrent logging from multiple tasks, lines could theoretically
// interleave at the newline boundary. This is acceptable for debug logging.
// ---------------------------------------------------------------------------

#ifndef RSERIAL_DEFAULT_BAUD
    #define RSERIAL_DEFAULT_BAUD 115200
#endif

#ifndef RSERIAL_MSG_BUFFER_SIZE
    #define RSERIAL_MSG_BUFFER_SIZE 128
#endif

#ifndef RSERIAL_QUEUE_LENGTH
    #define RSERIAL_QUEUE_LENGTH 16
#endif

#ifndef RSERIAL_TASK_STACK_SIZE
    #define RSERIAL_TASK_STACK_SIZE 4096
#endif

#ifndef RSERIAL_TASK_PRIORITY
    #define RSERIAL_TASK_PRIORITY 1
#endif

#ifndef RSERIAL_TASK_CORE
    #define RSERIAL_TASK_CORE 1
#endif

#ifndef RSERIAL_SEND_TIMEOUT_MS
    #define RSERIAL_SEND_TIMEOUT_MS 0
#endif

// Fixed-size message wrapper copied by value through the queue.
struct RSerialMessage
{
    char body[RSERIAL_MSG_BUFFER_SIZE];
};

class RSerial : public Print
{
private:
    Print* m_out;
    bool m_useDefaultSerial;

    QueueHandle_t m_queue = nullptr;
    TaskHandle_t m_taskHandle = nullptr;
    SemaphoreHandle_t m_lineMutex = nullptr;

    char m_lineBuf[RSERIAL_MSG_BUFFER_SIZE];
    size_t m_lineIdx = 0;

    // Copy the accumulated line into a message and push it to the queue, then
    // reset the line buffer. Must be called with m_lineMutex held.
    void enqueueLine()
    {
        if (m_lineIdx == 0) return;

        RSerialMessage msg;
        size_t len = m_lineIdx;
        if (len > RSERIAL_MSG_BUFFER_SIZE - 1) len = RSERIAL_MSG_BUFFER_SIZE - 1;
        memcpy(msg.body, m_lineBuf, len);
        msg.body[len] = '\0';

        xQueueSend(m_queue, (void *)&msg, pdMS_TO_TICKS(RSERIAL_SEND_TIMEOUT_MS));
        m_lineIdx = 0;
    }

    // Append one byte to the line buffer, flushing on newline or when full.
    // Must be called with m_lineMutex held.
    void appendByte(uint8_t c)
    {
        m_lineBuf[m_lineIdx++] = (char)c;

        if (c == '\n' || m_lineIdx >= RSERIAL_MSG_BUFFER_SIZE - 1)
        {
            enqueueLine();
        }
    }

    static void taskTrampoline(void *pvParameters)
    {
        static_cast<RSerial*>(pvParameters)->processQueue();
    }

    void processQueue()
    {
        RSerialMessage msg;
        while (true)
        {
            if (xQueueReceive(m_queue, (void *)&msg, portMAX_DELAY) == pdTRUE)
            {
                if (m_out != nullptr) m_out->print(msg.body);
            }
        }
    }

public:
    // Default: log to the global Serial; begin() will Serial.begin(baud).
    RSerial() : m_out(&Serial), m_useDefaultSerial(true) {}

    // Custom stream: the caller is responsible for calling .begin() on it.
    // Print* is the common base of Serial (USB CDC) and HardwareSerial.
    RSerial(Print* out) : m_out(out), m_useDefaultSerial(false) {}

    void begin(uint32_t baud = RSERIAL_DEFAULT_BAUD)
    {
        if (m_useDefaultSerial)
        {
            Serial.begin(baud);
        }

        m_lineMutex = xSemaphoreCreateMutex();
        m_queue = xQueueCreate(RSERIAL_QUEUE_LENGTH, sizeof(RSerialMessage));

        xTaskCreatePinnedToCore(
            taskTrampoline,
            "rSerial",
            RSERIAL_TASK_STACK_SIZE,
            this,
            RSERIAL_TASK_PRIORITY,
            &m_taskHandle,
            RSERIAL_TASK_CORE
        );
    }

    using Print::write;

    size_t write(uint8_t c) override
    {
        // Before begin(): write straight through so early-boot logs survive.
        if (m_queue == nullptr || m_lineMutex == nullptr)
        {
            return m_out != nullptr ? m_out->write(c) : 0;
        }

        xSemaphoreTake(m_lineMutex, portMAX_DELAY);
        appendByte(c);
        xSemaphoreGive(m_lineMutex);
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size) override
    {
        if (m_queue == nullptr || m_lineMutex == nullptr)
        {
            return m_out != nullptr ? m_out->write(buffer, size) : 0;
        }

        xSemaphoreTake(m_lineMutex, portMAX_DELAY);
        for (size_t i = 0; i < size; i++)
        {
            appendByte(buffer[i]);
        }
        xSemaphoreGive(m_lineMutex);
        return size;
    }
};

extern RSerial rSerial;
