#include "Arduino.h"
#include <RotaryEncoder.h>

class VolumeEncoder {
    private:

    RotaryEncoder *encoder;
    uint8_t m_encoderValue; // A value that ranges from 0 to 127 - following midi spec
    uint8_t m_encoderRotationScaling = 0; // A constant that will affect how many rotations needed to reach 0 or 127 

    public:
    VolumeEncoder(uint8_t enc_pinA, uint8_t enc_pinB, 
      uint8_t sw_pinAddr, 
      uint32_t sw_dbTime, uint8_t enc_rotScaling){
        encoder = new RotaryEncoder(enc_pinA, enc_pinB, 
      sw_pinAddr, sw_dbTime);
        m_encoderValue = 0;
        m_encoderRotationScaling = enc_rotScaling;
    };

    void begin(){
        encoder->begin();
        // encoder->attachLeftEventCallback(leftEventCallback);
        // encoder->attachRightEventCallback(rightEventCallback);

        encoder->attachLeftEventCallback([this](int count, void *usr_data){
        int value = this->getEncoderValue();

        if(count % 2 == 0){
            value += 1 + this->getEncoderRotationScaling() ;
            value = constrain(value, 0, 127);
            this->setEncoderValue(value);

            Serial.printf("Detect left event, new ESP32 count is %d\n", count);
            //Serial.printf("Detect Right event, new Encoder count is %d\n", value);
        }
        });
        encoder->attachRightEventCallback([this](int count, void *usr_data){
        int value = this->getEncoderValue();

        if(count % 2 == 0){
            value -= 1 + this->getEncoderRotationScaling() ;
            value = constrain(value, 0, 127);
            this->setEncoderValue(value);

            Serial.printf("Detect right event, new ESP32 count is %d\n", count);
            //Serial.printf("Detect Right event, new Encoder count is %d\n", value);
        }
        });
    }

    // returns the currently stored Encoder value
    uint8_t getEncoderValue(){
        //int ESP32encValue = encoder->getCount();
        //m_encoderValue = constrain(ESP32encValue, 0, 127);
        return m_encoderValue;
     };

     void setEncoderValue(int count){
        m_encoderValue = count;
     };

     uint8_t getEncoderRotationScaling(){ return m_encoderRotationScaling; }

    static void leftEventCallback(int count, void *usr_data){
        //VolumeEncoder *self = static_cast<VolumeEncoder *>(usr_data);
        //int value = self->getEncoderValue();

        if(count % 2 == 0){
            //value--;
            //value = constrain(value, 0, 127);
            Serial.printf("Detect left event, new ESP32 count is %d\n", count);
            //Serial.printf("Detect Right event, new Encoder count is %d\n", value);
        }
    };

    static void rightEventCallback(int count, void *usr_data){
        //VolumeEncoder *self = static_cast<VolumeEncoder *>(usr_data);
        //int value = self->getEncoderValue();

        if(count % 2 == 0){
            //value++;
            //value = constrain(value, 0, 127);
            Serial.printf("Detect Right event, new ESP32 count is %d\n", count);
            //Serial.printf("Detect Right event, new Encoder count is %d\n", value);
        }
    };

    bool sw_read(bool State){ return encoder->sw_read(State);};
    bool sw_wasPressed(){ return encoder->sw_wasPressed();};
};