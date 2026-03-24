#include <PushButton.h>
#include <BLUETOOTH_MIDI_STATE.h>
#pragma once


// Use the exact types the macro would generate
//extern bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE, bleMidi::DefaultSettings> BLEMIDI_bt;
//extern midi::MidiInterface<bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE, bleMidi::DefaultSettings>> MIDI_bt;

//extern bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE> BLEMIDI_bt;
//extern midi::MidiInterface<bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE>, bleMidi::MySettings> MIDI_bt;
class BluetoothModule
{
    private:
        static inline BluetoothModule* instance = nullptr;
        BLUETOOTH_MIDI_STATE* p_state = nullptr;

        PushButton* EnableButton = nullptr;

        uint8_t m_enablePin, m_ledPin;
        uint8_t m_timeForLongPress_ms = 150 /*ms*/;

        void (*ledWriteCallback)(uint8_t, uint8_t) = nullptr;

        unsigned long prevTime;

        // Pointers to our BLE-MIDI object - to be handled by the Midi Interface Manager
        //bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE> *bleTransport = nullptr;
        //midi::MidiInterface<bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE>, bleMidi::MySettings> *midiInterface = nullptr;
    
    public:

        BluetoothModule(uint8_t EnablePin, uint8_t LedPin, BLUETOOTH_MIDI_STATE* state): m_enablePin(EnablePin), m_ledPin(LedPin), p_state(state){
            EnableButton = new PushButton(EnablePin, 25 /*ms*/);
            instance = this;
        };

        template <typename T, typename I>
        void createInstance(T* transport, I* interface){
            // The following macro creates global objects named `BLEMIDI_bt` (transport) and `MIDI_bt` (interface).
            //bleTransport = transport;
            //midiInterface = interface;

            transport->setHandleConnected([]() {
                if(instance && instance->p_state){
                    Serial.println("---------CONNECTED---------");
                    instance->p_state->enable = true;
                    instance->digitalLedWrite(HIGH); } 
                });
            
            transport->setHandleDisconnected([]() {
                if(instance && instance->p_state){
                    Serial.println("---------CONNECTED---------");
                    instance->p_state->enable = false;
                    instance->digitalLedWrite(LOW); } 
                });
                
        }
        
        //using Transport = bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE, bleMidi::DefaultSettings>;
        //using Transport = bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE>;
        //auto* getTransport(){ return bleTransport; };

        //using Interface = midi::MidiInterface<bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE, bleMidi::DefaultSettings>> ;
        //using Interface = midi::MidiInterface<bleMidi::BLEMIDI_Transport<bleMidi::BLEMIDI_ESP32_NimBLE>, bleMidi::MySettings> ;
        //auto* getInterface(){ return midiInterface; };

        void setTimeForLongPress(uint8_t time_ms) { m_timeForLongPress_ms = time_ms; };
        uint8_t getTimeForLongPress(){ return m_timeForLongPress_ms; };
        
        void setDigitalLedWriteCallback(void (*digitalWrite)(uint8_t, uint8_t)){
            ledWriteCallback = digitalWrite;
        }

        void digitalLedWrite(uint8_t value);
        
        PushButton* getBluetoothPushButton(){
            if(EnableButton){
                return EnableButton;
            }
            
            Serial.println("Bluetooth Button not created");
            return nullptr;
        };

        enum ButtonMode{
            /* We require that as soon as a long press is detected, the BLE module starts to scan for devices
            When another long press is enabled then we need the BLE device to stop all scans and current connections*/
            BT_OFF,
            START_SCANNING,
            BLE_ENABLED,
        };

        ButtonMode MODE = BT_OFF;

        uint8_t buttonStateMachine(bool button_ISR_State);

        void stateMachine();

        // A method that sets the

};