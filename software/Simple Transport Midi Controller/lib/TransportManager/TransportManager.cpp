#include "TransportManager.h"
#include <string.h>


void TransportManager::createQueues()
{
    if (m_midiReadQueue == nullptr) {
        m_midiReadQueue = xQueueCreate(
            TRANSPORT_MANAGER_FREERTOS_EVENT_QUEUE_LENGTH,
            sizeof(MIDI_PACKET)
        );
    }

    // Prefer InputManager's existing outbound queue when it has already begun;
    // otherwise own the send queue (pass getMidiSendQueue() into InputManager::begin).
    if (m_midiSendQueue == nullptr) {
        if (inputMgr != nullptr && inputMgr->getMidiQueue() != nullptr) {
            m_midiSendQueue = inputMgr->getMidiQueue();
        } else {
            m_midiSendQueue = xQueueCreate(
                TRANSPORT_MANAGER_FREERTOS_EVENT_QUEUE_LENGTH,
                sizeof(MIDI_PACKET)
            );
        }
    }
}

void TransportManager::begin()
{
    instance = this;
    createQueues();
    attachListeners();

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

void TransportManager::vTransportManagerTask(void *pvParameters)
{
    TransportManager* mgr = static_cast<TransportManager*>(pvParameters);
    mgr->processTaskLoop();
}

void TransportManager::attachListeners()
{
    attachListeners(MIDI_usb);
    attachListeners(MIDI_ser);
    attachListeners(MIDI_bt);
}

void TransportManager::enqueueReadPacket(const MIDI_PACKET& packet)
{
    if (m_midiReadQueue == nullptr) {
        return;
    }

    if (xQueueSend(m_midiReadQueue, (void *)&packet, 0) != pdTRUE) {
        #ifdef TRANSPORT_MANAGER_DEBUG
            Serial.println("TransportManager: m_midiReadQueue full, dropping packet");
        #endif
    }
}

void TransportManager::HandleSysex(byte* array, unsigned size)
{
    if (instance == nullptr || array == nullptr || size < 3u) {
        return;
    }

    // FortySevenEffects delivers F0 ... F7 in the array
    if (array[0] != 0xF0 || array[1] != MIDI_SYSEX_MFR_ID || array[size - 1u] != 0xF7) {
        return;
    }

    const unsigned payloadSize = size - 3u; // Guarunteed by MIDI standard 
    if (payloadSize > MIDI_PACKET_SYSEX_MAX_PAYLOAD) {
        return;
    }

    MIDI_PACKET packet{};
    packet.type = MIDI_PACKET::TYPE::SYSEX;
    packet.interface = s_activeInterface;
    packet.sysexSize = payloadSize;
    if (payloadSize > 0u) {
        memcpy(packet.sysexData, &array[2], payloadSize);
    }

    instance->enqueueReadPacket(packet);
}

void TransportManager::handleControlChange(byte channel, byte number, byte value)
{
    if (instance == nullptr) {
        return;
    }

    MIDI_PACKET packet{};
    packet.type = MIDI_PACKET::TYPE::CC;
    packet.data1 = number;
    packet.data2 = value;
    packet.channel = channel;
    packet.interface = s_activeInterface;

    instance->enqueueReadPacket(packet);
}

void TransportManager::send(MIDI_PACKET* p)
{
    // Not implemented yet
    (void)p;
}

void TransportManager::read()
{
    /* s_activeInterface = MIDI_PACKET::INTERFACE::USB;
    MIDI_usb.read();

    s_activeInterface = MIDI_PACKET::INTERFACE::SER;
    MIDI_ser.read();

    s_activeInterface = MIDI_PACKET::INTERFACE::BLE;
    MIDI_bt.read();

     */
    
    s_activeInterface = MIDI_PACKET::INTERFACE::ANY;
    if (MIDI_usb.read()) {s_activeInterface = MIDI_PACKET::INTERFACE::USB;}
    if (MIDI_ser.read()) {s_activeInterface = MIDI_PACKET::INTERFACE::SER;}
    if (MIDI_bt.read()) {s_activeInterface = MIDI_PACKET::INTERFACE::BLE;}
}

void TransportManager::processTaskLoop()
{
    while (true)
    {
        if (m_midiSendQueue != nullptr &&
            xQueueReceive(m_midiSendQueue, (void *)&m_midiPacket, 0) == pdTRUE) {
            #ifdef TRANSPORT_MANAGER_DEBUG
                Serial.println("TransportManager: outbound MIDI packet received");
            #endif
            // send(&m_midiPacket); // not implemented yet
        }

        read();
    }
}
