#include <BluetoothModule.h>
#include <BLUETOOTH_MIDI_STATE.h>
#pragma once
class MIDI_IF
{
    private:
        BluetoothModule* btModule = nullptr;
        BLUETOOTH_MIDI_STATE* p_state = nullptr;

    public:
        MIDI_IF(BluetoothModule* module, BLUETOOTH_MIDI_STATE* state) : btModule(module), p_state(state){};
        /*void initialise()
        {
            if (btModule) {

                auto transport = btModule->getTransport();
                auto interface = btModule->getInterface();
            }
        };*/
        
        template <typename T, typename I>
        void stateMachine(T* transport, I* interface){
            if( p_state->scanning || !p_state->isInitialised){
                transport->begin();
                p_state->isInitialised = true;
            };

            if(p_state->isInitialised && p_state->enable){
                interface->read();
            };

            if(!p_state->enable && !p_state->scanning){
                transport->end();
                p_state->isInitialised = false;
            };

        };

        //auto getInterface(){ return btModule->getInterface(); };

        // A FREERTOS call back to continuously fire the read() command
        template <typename I>
        void FREERTOS_MIDI_Read_CB(I* interface){
            for (;;)
            {
                interface->read();
                vTaskDelay(1 / portTICK_PERIOD_MS); //Feed the watchdog of FreeRTOS.
            }
        };
};