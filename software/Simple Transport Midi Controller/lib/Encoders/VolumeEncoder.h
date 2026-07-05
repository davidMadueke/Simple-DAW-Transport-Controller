#include "Arduino.h"
#include <HAL_RotaryEncoder.h>

class VolumeEncoder {
    private:

    HAL_RotaryEncoder *encoder;


    uint8_t m_ledPin_R; // Pin number for the red LED
    uint8_t m_ledPin_G; // Pin number for the green LED
    uint8_t m_ledPin_B; // Pin number for the blue LED

    public:
    VolumeEncoder(
        uint8_t pinEnc1, uint8_t pinEnc2, uint8_t pinBtn, uint8_t dbTime,
        uint8_t ledPin_R, uint8_t ledPin_G, uint8_t ledPin_B
    ) 
        : m_ledPin_R(ledPin_R), m_ledPin_G(ledPin_G), m_ledPin_B(ledPin_B) 
    {
        encoder = new HAL_RotaryEncoder(pinEnc1, pinEnc2, pinBtn, dbTime);

    };

    void begin()
    {
        encoder->begin();
    }
};