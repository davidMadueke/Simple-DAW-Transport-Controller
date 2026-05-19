#include <MIDI_PACKET.h>
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
#include <BluetoothModule.h>
#include <MIDI.h>
#include <pins.h>
#include <Arduino.h>

#pragma once

#ifndef DEFINE_MIDI_MACRO
#include "deviceNames.h"

BLEMIDI_CREATE_INSTANCE(BT_DEVICE_NAME, MIDI_bt) 
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI_ser)


#define DEFINE_MIDI_MACRO 1
#endif

class TransportManager
{
    private:

    BluetoothModule* btModule = nullptr;
    


    public:
    TransportManager();

    void begin();
    void attachBleInterface(auto* bleMIDI);
    void attachUsbInterface(auto* usbMIDI);
    void attachSerialInterface(auto* serMIDI);
    void read();

    void send(MIDI_PACKET);

};