#include <MIDI_PACKET.h>
#include <InputManager.h>
#include <MIDI.h>
#include <pins.h>
#include <Arduino.h>



#pragma once

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

    //auto* btModule = nullptr;
    InputManager* inputMgr = nullptr;

    // FreeRTOS task
    MIDI_PACKET m_midiPacket;
    TaskHandle_t hdl_transportManagerTask = nullptr;


    public:
    TransportManager(InputManager* mgr) : inputMgr(mgr) {};

    void begin();
    static void vTransportManagerTask(void *pvParameters);
    void processTaskLoop();
    void attachBleInterface(auto* bleMIDI);
    void attachUsbInterface(auto* usbMIDI);
    void attachSerialInterface(auto* serMIDI);
    void read();

    void send(MIDI_PACKET* p);

};