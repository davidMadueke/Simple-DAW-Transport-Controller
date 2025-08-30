#include "LedRing_DUPPA.h"

LedRing::LedRing(uint8_t I2C_LedRing_Addr) {
    Ring = new LEDRingSmall(I2C_LedRing_Addr);
    m_startLedValue = 0;
}

void LedRing::setup() {
    Ring->LEDRingSmall_Reset();
  delay(20);

  Ring->LEDRingSmall_Configuration(0x01); //Normal operation
  Ring->LEDRingSmall_PWMFrequencyEnable(1);
  Ring->LEDRingSmall_SpreadSpectrum(0b0010110);
  Ring->LEDRingSmall_GlobalCurrent(0x04);
  Ring->LEDRingSmall_SetScaling(0xF0);
  Ring->LEDRingSmall_PWM_MODE();
}

void LedRing::reset(){
    Ring->LEDRingSmall_Reset();
}

void LedRing::clearAll(){
    Ring->LEDRingSmall_ClearAll();
}

void LedRing::clearLed(uint8_t logicalLEDPosition){
    int actualLedPosition = getLEDPosition(logicalLEDPosition);
    this->setLed(actualLedPosition, 0, 0, 0);
}

void LedRing::setLedStartValue(uint8_t startValue){
    m_startLedValue = startValue;
}

void LedRing::setLed(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b){
    int actualLedPosition = getLEDPosition(logicalLEDPosition);
    Ring->LEDRingSmall_Set_RED(actualLedPosition, r);
    Ring->LEDRingSmall_Set_GREEN(actualLedPosition, g);
    Ring->LEDRingSmall_Set_BLUE(actualLedPosition, b);
}

void LedRing::setLedFromState(uint8_t logicalLEDPosition){
    int actualLedPosition = getLEDPosition(logicalLEDPosition);
    Led_Colour_State lcs = getLedColourState(logicalLEDPosition);
    Ring->LEDRingSmall_Set_RED(actualLedPosition, lcs.red);
    Ring->LEDRingSmall_Set_GREEN(actualLedPosition, lcs.green);
    Ring->LEDRingSmall_Set_BLUE(actualLedPosition, lcs.blue );
}


void LedRing::dial_setLed(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b){
    int actualLedPosition;
    for(int i = m_startLedValue; i <= logicalLEDPosition; i++){
        actualLedPosition = this->getLEDPosition(i);
        this->setLed(actualLedPosition, r, g, b);
    }

   /* for(int i = logicalLEDPosition; i <= TOTAL_LEDS - 1; i++){
        actualLedPosition = this->getLEDPosition(i);
        this->clearLed(actualLedPosition);
    }*/
}

void LedRing::dial_setLedFromState(uint8_t logicalLEDPosition){
    int actualLedPosition;
    for(int i = m_startLedValue; i <= logicalLEDPosition; i++){
        actualLedPosition = this->getLEDPosition(i);
        this->setLedFromState(actualLedPosition);
    }

    for(int i = logicalLEDPosition; i <= TOTAL_LEDS - 1; i++){
        actualLedPosition = this->getLEDPosition(i);
        this->clearLed(actualLedPosition);
    }
}