#include "LedRing.h"

LedRing::LedRing(uint8_t I2C_LedRing_Addr) {
    LedRing = new LEDRingSmall(I2C_LedRing_Addr);
}

LedRing::setup() {
    LedRing->LEDRingSmall_Reset();
  delay(20);

  LedRing->LEDRingSmall_Configuration(0x01); //Normal operation
  LedRing->LEDRingSmall_PWMFrequencyEnable(1);
  LedRing->LEDRingSmall_SpreadSpectrum(0b0010110);
  LedRing->LEDRingSmall_GlobalCurrent(0x04);
  LedRing->LEDRingSmall_SetScaling(0xF0);
  LedRing->LEDRingSmall_PWM_MODE();
}

LedRing::reset(){
    LedRing->LEDRingSmall_reset();
}

LedRing::setLedStartValue(uint8_t startValue){
    startLedValue = startValue;
}

void setLed(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b){
    int actualLedPosition = getLEDPosition(logicalLEDPosition);
    LEDRingSmall.LEDRingSmall_Set_RED(actualLedPosition, r);
    LEDRingSmall.LEDRingSmall_Set_GREEN(actualLedPosition, g);
    LEDRingSmall.LEDRingSmall_Set_BLUE(actualLedPosition, b);
}