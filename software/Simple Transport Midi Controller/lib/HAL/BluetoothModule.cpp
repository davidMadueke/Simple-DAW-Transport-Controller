#include "BluetoothModule.h"
#include <BLUETOOTH_MIDI_STATE.h>

void BluetoothModule::digitalLedWrite(uint8_t value){
    if (ledWriteCallback) {
                ledWriteCallback(m_ledPin, value);
            } else {
                Serial.println("Error: ledWriteCallback is not set.");
                return;
            }
};

void BluetoothModule::buttonStateMachine(bool button_ISR_State, BLUETOOTH_MIDI_STATE* state){
    PushButton* button = getBluetoothPushButton();
    

    switch (MODE)
    {
    case BT_OFF:
        if (button->pressedFor(m_timeForLongPress_ms)){
            MODE = START_SCANNING;
            break;
        }
        break;
    
    case START_SCANNING:
        break;
    
    case BLE_ENABLED:
        break;
    
    case toBT_OFF:
        break;

    default:
        Serial.println("Undefined Bluetooth Module State Machine Behaviour");
        break;
    }


}