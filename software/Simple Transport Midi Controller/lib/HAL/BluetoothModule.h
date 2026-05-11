#include <PushButton.h>
#include <BLUETOOTH_MIDI_STATE.h>
#pragma once


class BluetoothModule
{
    private:
        static inline BluetoothModule* instance = nullptr;
        BLUETOOTH_MIDI_STATE* p_state = nullptr;


        uint8_t m_enablePin, m_ledPin;
        uint8_t m_timeForLongPress_ms = 150 /*ms*/;

        void (*ledWriteCallback)(uint8_t, uint8_t) = nullptr;

        unsigned long prevTime;

    
    public:

        BluetoothModule(uint8_t LedPin, BLUETOOTH_MIDI_STATE* state): m_ledPin(LedPin), p_state(state){
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
        

        void setTimeForLongPress(uint8_t time_ms) { m_timeForLongPress_ms = time_ms; };
        uint8_t getTimeForLongPress(){ return m_timeForLongPress_ms; };
        
        void setDigitalLedWriteCallback(void (*digitalWrite)(uint8_t, uint8_t)){
            ledWriteCallback = digitalWrite;
        }

        void digitalLedWrite(uint8_t value);
    

};