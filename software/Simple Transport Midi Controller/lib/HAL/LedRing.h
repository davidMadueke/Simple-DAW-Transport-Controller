#include <Arduino.h>
#include <Wire.h>
#include <LEDRingSmall.h>


/**
 * @class LedRing
 * @brief A class that represents an RGB I2C LED rING
 */

 class LedRing {
    private:
    LedRingSmall *LedRing;
    static const int TOTAL_LEDS = 25;
    int startLedValue;

    // Helper function to map logical LED position to physical LED position
    int getLEDPosition(int logicalPosition) const {
        return (startLedValue + logicalPosition) % TOTAL_LEDS;
    }

    public:
    LedRing(uint8_t I2C_LedRing_Addr) : startLedValue(0);

    // Calls all of the setup functions found in the setup function in the DuPPaLib LEDRingSmall_Demo example sketch
    // https://github.com/Fattoresaimon/ArduinoDuPPaLib/tree/ba86fe918f2906e64399f8f414f0bb433a502234/examples/RGB%20LED%20Ring%20Small/LEDRingSmall_Demo
    setup();

    // Resets all the LEDs to the default state
    reset();

    //Set a value (between 0 and 24) that will serve as the 1st LED in the Array
    //Any calls of other functions will convert that corresponding value arg to the respective element of the set (startValue, startValue - 1) mod 24
    setLedStartValue(uint8_t startValue);
    
    void setLed(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b);

 }