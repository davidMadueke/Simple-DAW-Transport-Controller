#include "UsbMidiModule.h"

void UsbMidiModule::begin(){
    if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  _usb_midi.setStringDescriptor(USB_DEVICE_NAME);
  
  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  // Taken from TinyUsb Midi Example
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  
}

bool UsbMidiModule::isMounted(){
    return TinyUSBDevice.mounted();
}