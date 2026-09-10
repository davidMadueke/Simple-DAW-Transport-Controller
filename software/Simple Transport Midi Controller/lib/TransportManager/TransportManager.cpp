#include "TransportManager.h"
#include "deviceNames.h"
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
#include <Adafruit_TinyUSB.h>
#include <string.h>

BLEMIDI_CREATE_INSTANCE(BT_DEVICE_NAME, MIDI_bt);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial0, MIDI_ser);
Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI_usb);

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

void TransportManager::begin(uint8_t bleChannel, uint8_t serChannel, uint8_t usbChannel)
{
    instance = this;

    createQueues();

    if (bleModule != nullptr) {
        bleModule->createInstance(&BLEMIDI_bt, &MIDI_bt);
    }

    if (!TinyUSBDevice.isInitialized()) {
        TinyUSBDevice.begin(0);
    }
    usb_midi.setStringDescriptor(USB_DEVICE_NAME);

    /* MIDI_usb.begin(usbChannel); */
    MIDI_ser.begin(serChannel);
    MIDI_bt.begin(bleChannel);

    /* if (TinyUSBDevice.mounted()) {
        TinyUSBDevice.detach();
        delay(10);
        TinyUSBDevice.attach();
    }
 */
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

void TransportManager::handleSysex(byte* array, unsigned size)
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
    switch (p->type)
    {
        case MIDI_PACKET::TYPE::CC:
            sendControlChange(MIDI_usb, p->data1, p->data2, p->channel);    
            sendControlChange(MIDI_ser, p->data1, p->data2, p->channel);    
            sendControlChange(MIDI_bt, p->data1, p->data2, p->channel);      
            break;
        
        // NOTE: Endless Encoder msgs are just CC msgs with discrete predefined ccValue ranges (see Endless Encoder Mode Architecture diagram)
        case MIDI_PACKET::TYPE::ENDLESS_ENCODER:
            sendControlChange(MIDI_usb, p->data1, p->data2, p->channel);    
            sendControlChange(MIDI_ser, p->data1, p->data2, p->channel);    
            sendControlChange(MIDI_bt, p->data1, p->data2, p->channel);      
            break;

        case MIDI_PACKET::TYPE::NOTE_ON:
            sendNoteOn(MIDI_usb, p->data1, p->data2, p->channel);           
            sendNoteOn(MIDI_ser, p->data1, p->data2, p->channel);           
            sendNoteOn(MIDI_bt,  p->data1, p->data2, p->channel);                
            break;
        
        // If Note Off is detected send a noteOn with zero Velocity (check FortySevenEffects MIDI API reference)
        case MIDI_PACKET::TYPE::NOTE_OFF:
            sendNoteOn(MIDI_usb, p->data1, 0u, p->channel);                 
            sendNoteOn(MIDI_ser, p->data1, 0u, p->channel);                 
            sendNoteOn(MIDI_bt,  p->data1, 0u, p->channel);                   
            break;
        
        case MIDI_PACKET::TYPE::SYSEX:
            sendSysex(MIDI_usb, p->sysexSize, p->sysexData);                
            sendSysex(MIDI_ser, p->sysexSize, p->sysexData);                
            sendSysex(MIDI_bt,  p->sysexSize, p->sysexData);                
            break;
        
        default:
            break;
    } 
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
            xQueueReceive(
                m_midiSendQueue,
                (void *)&m_midiPacket,
                pdMS_TO_TICKS(TRANSPORT_MANAGER_QUEUE_RECEIVE_TIMEOUT_MS)
            ) == pdTRUE) {
            #ifdef TRANSPORT_MANAGER_DEBUG
                Serial.printf(
                    "TransportManager: outbound MIDI type=%u data1=%u data2=%u ch=%u\n",
                    static_cast<unsigned>(m_midiPacket.type),
                    m_midiPacket.data1,
                    m_midiPacket.data2,
                    m_midiPacket.channel
                );
            #endif
            send(&m_midiPacket);
        }

        read();
    }
}
