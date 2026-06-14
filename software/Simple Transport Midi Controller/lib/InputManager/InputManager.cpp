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

void InputManager::begin()
{
    m_inputSet = xQueueCreateSet(m_queueSetLength);
    for (size_t i = 0; i < m_inputCount; ++i) {
        QueueHandle_t handle = m_inputs[i].queue;
        xQueueAddToSet(handle, m_inputSet);
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
     for( ;; )
    {
        /* Block to wait for something to be available from the queues or
           semaphore that have been added to the set. Don't block longer than
           200ms. */
        QueueSetMemberHandle_t xActivatedMember = xQueueSelectFromSet( m_inputSet,
             200 / portTICK_PERIOD_MS );

        if (xActivatedMember == nullptr) { continue;}

        for (size_t i = 0; i < m_inputCount; ++i)
        {
            if (xActivatedMember != m_inputs[i].queue) continue;
            switch (m_inputs[i].type) {
                case InputSource::Type::RgbButton:
                    dispatchButtonEvent(m_inputs[i].queue);
                    break;
                case InputSource::Type::TapTempo:
                    dispatchTapTempoEvent(m_inputs[i].queue);
                    break;
                case InputSource::Type::Encoder:
                    // dispatchEncoderEvent(m_inputs[i].queue);
                    break;
            }
        }
    }
}