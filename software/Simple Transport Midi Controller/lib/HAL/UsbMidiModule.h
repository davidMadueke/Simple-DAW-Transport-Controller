#include <Adafruit_TinyUSB.h>
#include "deviceNames.h"

#pragma once

class UsbMidiModule
{
    private:
        Adafruit_USBD_MIDI _usb_midi;
    public:
        void begin();

        bool isMounted();

        Adafruit_USBD_MIDI& getInstance(){
            return _usb_midi;
        };
};