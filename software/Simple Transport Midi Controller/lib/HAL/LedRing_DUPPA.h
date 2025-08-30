#include <Arduino.h>
#include <Wire.h>
#include <LEDRingSmall.h>


/**
 * @class LedRing
 * @brief A class that represents an RGB I2C LED rING
 */

 class LedRing {
    public:
    /**
     * @struct Led_Colour_State
     * @brief Stores RGB values for a single LED
     */
    struct Led_Colour_State {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    private:
    LEDRingSmall *Ring;
    static const int TOTAL_LEDS = 25;
    int m_startLedValue;

//    uint8_t m_LedColourState_red; // Used to keep track of the LED non-off state
//    uint8_t m_LedColourState_green;
//    uint8_t m_LedColourState_blue;

    // Array to store colour state for each LED
    Led_Colour_State m_ledColourStates[TOTAL_LEDS];


    public:
    LedRing(uint8_t I2C_LedRing_Addr);

    // Helper function to map logical LED position to physical LED position
    int getLEDPosition(int logicalPosition) const {
        return (m_startLedValue + logicalPosition) % TOTAL_LEDS;
    }
    
    int getMaxLEDPosition() const {
        return TOTAL_LEDS - 1; // As array logic is zero-indexed
    }

    // Calls all of the setup functions found in the setup function in the DuPPaLib LEDRingSmall_Demo example sketch
    // https://github.com/Fattoresaimon/ArduinoDuPPaLib/tree/ba86fe918f2906e64399f8f414f0bb433a502234/examples/RGB%20LED%20Ring%20Small/LEDRingSmall_Demo
    void setup();

    // Resets all the LEDs to the default state
    void reset();

     // Turns of  all the LEDs to the default state
    void clearAll();

    // Turns of the chosen LEDs to the default state
    void clearLed(uint8_t logicalLEDPosition);

    //Set a value (between 0 and 24) that will serve as the 1st LED in the Array
    //Any calls of other functions will convert that corresponding value arg to the respective element of the set (startValue, startValue - 1) mod 24
    void setLedStartValue(uint8_t startValue);
    
    // Sets a specific LED in the ring array to light up, unaffecting all the others
    void setLed(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b);

    // Sets a specific LED in the ring array to light up, unaffecting all the others
    // Uses the internally stored LED colour states
    void setLedFromState(uint8_t logicalLEDPosition);

    // Sets all LEDs before and including the logical LED Position to the same given RGB values
    void dial_setLed(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b);
    
    // Sets all LEDs before and including the logical LED Position to the same given RGB values    
    // Uses the internally stored LED colour states
    void dial_setLedFromState(uint8_t logicalLEDPosition);

    // Retrieve the colour state of a chosen logical LED position
    Led_Colour_State getLedColourState(uint8_t logicalLEDPosition) const {
        int pos = getLEDPosition(logicalLEDPosition);
        return m_ledColourStates[pos];
    }

    // Change the colour state of a chosen logical LED position
    void setLedColourState(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b) {
        int pos = getLEDPosition(logicalLEDPosition);
        Led_Colour_State newState = {r, g, b};
        m_ledColourStates[pos] = newState;
    }
};