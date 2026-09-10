#pragma once
#include <Arduino.h>
#include <MIDI_PACKET.h>
#include <DisplayAction.h>
#include <InputCompiler.h>
#include <InputSource.h>

#ifndef INPUT_MANAGER_MAX_INPUT_SOURCES
    #define INPUT_MANAGER_MAX_INPUT_SOURCES 16u
#endif

#ifndef INPUT_MANAGER_FREERTOS_PRIORITY
    #define INPUT_MANAGER_FREERTOS_PRIORITY 5
#endif
#ifndef INPUT_MANAGER_FREERTOS_TASK_STACK_SIZE
    #define INPUT_MANAGER_FREERTOS_TASK_STACK_SIZE 4096
#endif

#ifndef INPUT_MANAGER_FREERTOS_OUTPUT_QUEUES_LENGTH
    #define INPUT_MANAGER_FREERTOS_OUTPUT_QUEUES_LENGTH 8
#endif



class InputManager
{
    private:

    size_t m_queueSetLength = 0;
    TaskHandle_t hdl_InputMgrTask = nullptr;
    QueueSetHandle_t m_inputSet = nullptr;
    QueueHandle_t m_midiQueue = nullptr;
    QueueHandle_t m_displayQueue = nullptr;


    static constexpr size_t kMaxInputs = INPUT_MANAGER_MAX_INPUT_SOURCES;
    InputSource m_inputs[kMaxInputs] = {};
    size_t m_inputCount = 0;



    void inputManagerTaskLoop();

    public:
    //InputManager(){};

    void begin(QueueHandle_t midiQueue = nullptr, QueueHandle_t displayQueue = nullptr);

    static void vInputManagerTask(void* pvParameters);

    void registerInputSource(InputSource input);

    void loop();

    QueueHandle_t getMidiQueue() { return m_midiQueue;};

    QueueHandle_t getDisplayQueue() { return m_displayQueue;};

};