#pragma once

#include <MIDI_PACKET.h>
#include <InputManager.h>
#include <BluetoothModule.h>
#include <MIDI.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#ifndef MIDI_SYSEX_MFR_ID
    #define MIDI_SYSEX_MFR_ID 0x7D
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
    public:


    TransportManager(InputManager* mgr, BluetoothModule* ble = nullptr)
        : inputMgr(mgr), bleModule(ble) {};

    private:

    // Static instance for FortySevenEffects MIDI.read callbacks
    static inline TransportManager* instance = nullptr;

    // Stamped around each interface read() so shared handlers know the source
    static inline MIDI_PACKET::INTERFACE s_activeInterface = MIDI_PACKET::INTERFACE::ANY;

    InputManager* inputMgr = nullptr;
    BluetoothModule* bleModule = nullptr;

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
        midi.setHandleSystemExclusive(handleSysex);
        midi.setHandleControlChange(handleControlChange);
    }

    static void handleSysex(byte* array, unsigned size);
    static void handleControlChange(byte channel, byte number, byte value);

    public:

    void begin(uint8_t bleChannel = MIDI_CHANNEL_OMNI,
                uint8_t serChannel = MIDI_CHANNEL_OMNI,
                uint8_t usbChannel = MIDI_CHANNEL_OMNI);
    static void vTransportManagerTask(void *pvParameters);
    void processTaskLoop();

    void attachListeners();
    void read();

    void send(MIDI_PACKET* p);

    template <typename MidiT>
    void sendControlChange(MidiT& midi, byte ccNumber, byte ccValue, byte channel)
    {
        midi.sendControlChange(ccNumber, ccValue, channel);
    }

    template <typename MidiT>
    //Note: you can send NoteOn with zero velocity to make a NoteOff, this is based on the Running Status principle, to avoid sending status messages and thus sending only NoteOn data. sendNoteOff will always send a real NoteOff message. Take a look at the values, names and frequencies of notes here: http://www.phys.unsw.edu.au/jw/notes.html
    void sendNoteOn(MidiT& midi, byte inNoteNumber, byte inVelocity, byte channel)
    {
        midi.sendNoteOn(inNoteNumber, inVelocity, channel);
    }

    template <typename MidiT>
    void sendSysex(MidiT& midi, unsigned inLength, const byte* inArray)
    {
        midi.sendSysEx(inLength, inArray);
    }
    QueueHandle_t getMidiReadQueue() { return m_midiReadQueue; }
    QueueHandle_t getMidiSendQueue() { return m_midiSendQueue; }
};
