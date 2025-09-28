#include "BluetoothModule.h"

void BluetoothModule::digitalLedWrite(uint8_t value){
    if (ledWriteCallback) {
                ledWriteCallback(m_ledPin, value);
            } else {
                Serial.println("Error: ledWriteCallback is not set.");
                return;
            }
}