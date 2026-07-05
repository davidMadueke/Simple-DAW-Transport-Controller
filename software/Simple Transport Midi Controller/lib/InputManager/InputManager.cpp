#include "InputManager.h"

void InputManager::registerInputSource(InputSource input)
{
    if (input.queue == nullptr) return;
    if(m_inputCount < kMaxInputs) {
        QueueHandle_t queue = input.queue;
        m_queueSetLength += uxQueueSpacesAvailable(queue) + uxQueueMessagesWaiting(queue);
        m_inputs[m_inputCount] = input;
        m_inputCount++;
        configASSERT( queue );
    };

}

void InputManager::begin(QueueHandle_t midiQueue, QueueHandle_t displayQueue)
{
    m_inputSet = xQueueCreateSet(m_queueSetLength);
    for (size_t i = 0; i < m_inputCount; ++i) 
    {
        QueueHandle_t handle = m_inputs[i].queue;
        xQueueAddToSet(handle, m_inputSet);
    }


    if (midiQueue != nullptr) 
    {
            m_midiQueue = midiQueue;

    } else {

        m_midiQueue = xQueueCreate(
        INPUT_MANAGER_FREERTOS_OUTPUT_QUEUES_LENGTH,
        sizeof(MIDI_PACKET)
        );
    }

    if (displayQueue != nullptr) 
    {
            m_displayQueue = displayQueue;
            
    } else {

        m_displayQueue = xQueueCreate(
        INPUT_MANAGER_FREERTOS_OUTPUT_QUEUES_LENGTH,
        sizeof(DISPLAY_ACTION)
        );
    }

    xTaskCreate(vInputManagerTask,
         "input_mgr", 
         INPUT_MANAGER_FREERTOS_TASK_STACK_SIZE, 
         this, 
         INPUT_MANAGER_FREERTOS_PRIORITY, 
         &hdl_InputMgrTask);
    
}


void InputManager::vInputManagerTask(void* pvParameters) 
{
    static_cast<InputManager*>(pvParameters)->inputManagerTaskLoop();
}

void InputManager::inputManagerTaskLoop()
{
    QueueSetMemberHandle_t xActivatedMember;
    for( ;; )
    {
        /* Block to wait for something to be available from the queues or
           semaphore that have been added to the set. Don't block longer than
           200ms. */
        xActivatedMember = xQueueSelectFromSet( m_inputSet,
             200 / portTICK_PERIOD_MS );

        if (xActivatedMember == nullptr) { continue;}

        for (size_t i = 0; i < m_inputCount; ++i)
        {
            if (xActivatedMember != m_inputs[i].queue) continue;

            inputDispatcher(m_inputs[i], getMidiQueue(), getDisplayQueue());
            
        }
    }
}