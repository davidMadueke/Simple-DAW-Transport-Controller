#include "Arduino.h"
#include <RotaryEncoder.h>

class InfScrollEncoder {
    private:
    RotaryEncoder *encoder;
    uint8_t m_encoderValue; // A value that ranges from 0 to 127 - following midi spec
    uint8_t m_ledState_R; // Current state of the red LED
    uint8_t m_ledState_G; // Current state of the green LED
    uint8_t m_ledState_B; // Current state of the blue LED

    uint8_t m_ledPin_R; // Pin number for the red LED
    uint8_t m_ledPin_G; // Pin number for the green LED
    uint8_t m_ledPin_B; // Pin number for the blue LED

    public:
    InfScrollEncoder(uint8_t ledPin_R, uint8_t ledPin_G, uint8_t ledPin_B) 
        : m_ledPin_R(ledPin_R), m_ledPin_G(ledPin_G), m_ledPin_B(ledPin_B) {
        int dummyPin1, dummyPin2 = 0; // As RotEncoder not connected to mcu directly set up dummy pins for RotaryEncoder class
        encoder = new RotaryEncoder(dummyPin1, dummyPin2);
        m_encoderValue = 0;
    };

    // If no args, simply returns the currently stored Encoder value
    uint8_t getEncoderValue(){ return m_encoderValue; };

    uint8_t processEncoderValue(unsigned char pin1State, unsigned char pin2State){
        m_encoderValue = encoder->process(pin1State, pin2State);
        return m_encoderValue;
    };

    void ledSetup() {
        pinMode(m_ledPin_R, OUTPUT);
        pinMode(m_ledPin_G, OUTPUT);
        pinMode(m_ledPin_B, OUTPUT);
    };

    void setLedState(uint8_t red, uint8_t green, uint8_t blue) {
        m_ledState_R = red;
        m_ledState_G = green;
        m_ledState_B = blue;

    };
        
    void setLeds()
    {
        analogWrite(m_ledPin_R, m_ledState_R);
        analogWrite(m_ledPin_G, m_ledState_G);
        analogWrite(m_ledPin_B, m_ledState_B);
    }
};