#include "TransportManager.h"


void TransportManager::begin(){
        xTaskCreatePinnedToCore(
            vTransportManagerTask,
            "transportMain",
            TRANSPORT_MANAGER_FREERTOS_TASK_STACK_SIZE,
            this,
            TRANSPORT_MANAGER_FREERTOS_PRIORITY,
            &hdl_transportManagerTask,
            1
        );
}

void TransportManager::vTransportManagerTask(void *pvParameters){
    TransportManager* instance = static_cast<TransportManager*>(pvParameters);
    instance->processTaskLoop();
}

// Create FREE RTOS Read Task that will handle the reads

void TransportManager::send(MIDI_PACKET* p){
    // For now implement it where the system loops through all transports and calls send
}

void TransportManager::read(){
    MIDI_bt.read();
    MIDI_ser.read();
    MIDI_bt.read();
}

void TransportManager::processTaskLoop(){

    while(true)
    {
        if (xQueueReceive(inputMgr->getMidiQueue(), (void *)&m_midiPacket, 0) == pdTRUE) {
                #ifdef TRANSPORT_MANAGER_DEBUG
                    Serial.printf("Debug for %s, line 37 of Transport Manager. Queue Empty", taskName);
                #endif
                //stateMachine(&event);
        }
        read();
    }
}