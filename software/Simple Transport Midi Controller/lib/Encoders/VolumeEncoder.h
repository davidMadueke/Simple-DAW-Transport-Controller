#include "Arduino.h"
#include <RotaryEncoder.h>

class VolumeEncoder {
    private:

    RotaryEncoder *encoder;
    uint8_t m_encoderValue; // A value that ranges from 0 to 127 - following midi spec

    public:
    VolumeEncoder(){
        int dummyPin1, dummyPin2 = 0; // As RotEncoder not connected to mcu directly set up dummy pins for RotaryEncoder class
        encoder = new RotaryEncoder(dummyPin1, dummyPin2);
        m_encoderValue = 0;
    };

    // If no args, simply returns the currently stored Encoder value
    uint8_t getEncoderValue(){ return m_encoderValue; };

    uint8_t processEncoderValue(unsigned char pin1State, unsigned char pin2State){
        m_encoderValue = encoder->process(pin1State, pin2State);
        return m_encoderValue;
    }
};