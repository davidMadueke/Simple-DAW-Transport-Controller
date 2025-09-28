#include <PushButton.h>

class BluetoothModule
{
    private:
        PushButton* EnableButton;

        uint8_t m_enablePin, m_ledPin;

        void (*ledWriteCallback)(uint8_t, uint8_t) = nullptr;
    
    public:
        BluetoothModule(uint8_t EnablePin, uint8_t LedPin){

        };

        void begin();

        void update();

        void setDigitalLedWriteCallback(void (*digitalWrite)(uint8_t, uint8_t)){
            ledWriteCallback = digitalWrite;
        }

        void digitalLedWrite(uint8_t value);

};