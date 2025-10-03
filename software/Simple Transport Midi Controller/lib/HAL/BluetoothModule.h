#include <PushButton.h>
#include <BLUETOOTH_MIDI_STATE.h>

class BluetoothModule
{
    private:
        PushButton* EnableButton = nullptr;

        uint8_t m_enablePin, m_ledPin;
        uint8_t m_timeForLongPress_ms = 150;

        void (*ledWriteCallback)(uint8_t, uint8_t) = nullptr;
        void (*BLE_MidiCallback)() = nullptr;

        bool isConnected = false;
    
    public:
        BluetoothModule(uint8_t EnablePin, uint8_t LedPin){

        };

        void begin();

        void update();

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
            toBT_OFF,
        };

        ButtonMode MODE = BT_OFF;

        void buttonStateMachine(bool button_ISR_State, BLUETOOTH_MIDI_STATE* state);

        // A method that sets the

};