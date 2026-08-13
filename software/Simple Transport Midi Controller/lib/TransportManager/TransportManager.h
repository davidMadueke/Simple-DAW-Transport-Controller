#pragma once

#include <MIDI_PACKET.h>
#include <InputManager.h>
#include <MIDI.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#ifndef MIDI_SYSEX_MFR_ID
    #define MIDI_SYSEX_MFR_ID 0x7D
#endif

#ifndef DEFINE_MIDI_MACRO
#include "deviceNames.h"
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
#include <Adafruit_TinyUSB.h>


BLEMIDI_CREATE_INSTANCE(BT_DEVICE_NAME, MIDI_bt);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial0, MIDI_ser);
Adafruit_USBD_MIDI usb_midi;

MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI_usb);

#define DEFINE_MIDI_MACRO 1
#endif

#ifndef TRANSPORT_MANAGER_FREERTOS_PRIORITY
    #define TRANSPORT_MANAGER_FREERTOS_PRIORITY 3
#endif

#ifndef TRANSPORT_MANAGER_FREERTOS_TASK_STACK_SIZE
    #define TRANSPORT_MANAGER_FREERTOS_TASK_STACK_SIZE 4096
#endif

#ifndef TRANSPORT_MANAGER_FREERTOS_EVENT_QUEUE_LENGTH
    #define TRANSPORT_MANAGER_FREERTOS_EVENT_QUEUE_LENGTH 8
#endif

#ifndef TRANSPORT_MANAGER_QUEUE_RECEIVE_TIMEOUT_MS
    #define TRANSPORT_MANAGER_QUEUE_RECEIVE_TIMEOUT_MS 200
#endif


class TransportManager
{
    private:

    // Static instance for FortySevenEffects MIDI.read callbacks
    static inline TransportManager* instance = nullptr;

    // Stamped around each interface read() so shared handlers know the source
    static inline MIDI_PACKET::INTERFACE s_activeInterface = MIDI_PACKET::INTERFACE::ANY;

    InputManager* inputMgr = nullptr;

    // FreeRTOS task
    MIDI_PACKET m_midiPacket;
    TaskHandle_t hdl_transportManagerTask = nullptr;

    // Inbound (host -> device) and outbound (device -> host) queues
    QueueHandle_t m_midiReadQueue = nullptr;
    QueueHandle_t m_midiSendQueue = nullptr;

    void createQueues();
    void enqueueReadPacket(const MIDI_PACKET& packet);

    template <typename MidiT>
    void attachListeners(MidiT& midi) {
        midi.setHandleSystemExclusive(HandleSysex);
        midi.setHandleControlChange(handleControlChange);
    }

    static void HandleSysex(byte* array, unsigned size);
    static void handleControlChange(byte channel, byte number, byte value);

    public:

    TransportManager(InputManager* mgr) : inputMgr(mgr) {};

    void begin();
    static void vTransportManagerTask(void *pvParameters);
    void processTaskLoop();

    void attachListeners();
    void read();

    void send(MIDI_PACKET* p);

    QueueHandle_t getMidiReadQueue() { return m_midiReadQueue; }
    QueueHandle_t getMidiSendQueue() { return m_midiSendQueue; }
};
