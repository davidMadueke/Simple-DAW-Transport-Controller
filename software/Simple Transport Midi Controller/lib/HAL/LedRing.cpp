#include "LedRing.h"

LedRing::LedRing(uint8_t I2C_LedRing_Addr) {
    Ring = new LEDRingSmall(I2C_LedRing_Addr);
    startLedValue = 0;
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

void LedRing::setLedStartValue(uint8_t startValue){
    startLedValue = startValue;
}

void LedRing::setLed(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b){
    int actualLedPosition = getLEDPosition(logicalLEDPosition);
    Ring->LEDRingSmall_Set_RED(actualLedPosition, r);
    Ring->LEDRingSmall_Set_GREEN(actualLedPosition, g);
    Ring->LEDRingSmall_Set_BLUE(actualLedPosition, b);
}