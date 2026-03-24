#include "BluetoothModule.h"
//#include "deviceNames.h"
//BLEMIDI_CREATE_INSTANCE(BT_DEVICE_NAME, MIDI_bt)  
// 1. Manually define the Transport (this allocates the memory)
//bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE> BLEMIDI_bt(BT_DEVICE_NAME);

// 2. Manually define the MIDI Interface, passing the Transport as a reference
//midi::MidiInterface<bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE>, bleMidi::MySettings> MIDI_bt((bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE>&)BLEMIDI_bt);

void BluetoothModule::digitalLedWrite(uint8_t value){
    if (ledWriteCallback) {
                ledWriteCallback(m_ledPin, value);
            } else {
                Serial.println("Error: ledWriteCallback is not set.");
                return;
            };
};


uint8_t BluetoothModule::buttonStateMachine(bool button_ISR_State){
    PushButton* button = getBluetoothPushButton();
    uint8_t numOfPresses = button->multiPressRead(button_ISR_State);
    //digitalLedWrite(p_state->enable ? HIGH : LOW);

    switch (MODE)
    {
    case BT_OFF:
        if (button->pressedFor(m_timeForLongPress_ms)){
            MODE = START_SCANNING;
            break;
        }

        p_state->scanning = false;
        break;
    
    case START_SCANNING:
        if(p_state->enable){
            MODE = BLE_ENABLED;
        }

        static bool ledState = LOW;

        if (millis() - prevTime > 1000) {
            ledState = !ledState;
            digitalLedWrite(ledState);
            prevTime = millis();
        }

        p_state->scanning = true;
        MODE = START_SCANNING;
        break;
    
    case BLE_ENABLED:
        Serial.println("BLE Device Connected");

        if(p_state->enable){
            MODE = BLE_ENABLED;
        }
        
        if (button->pressedFor(m_timeForLongPress_ms)){
            p_state->enable = false;
            MODE = BT_OFF;
        }
        p_state->scanning = false;
        break;
    
    default:
        Serial.println("Undefined Bluetooth Module State Machine Behaviour");
        break;
    }

    return numOfPresses;

}