#include <Arduino.h>
#include <Wire.h>
#include <LEDRingSmall.h>
#include <freertos/queue.h>
#pragma once

#ifndef LED_RING_DIAL_FREERTOS_PRIORITY
    #define LED_RING_DIAL_FREERTOS_PRIORITY 3
#endif

#ifndef LED_RING_DIAL_FREERTOS_TASK_STACK_SIZE
    #define LED_RING_DIAL_FREERTOS_TASK_STACK_SIZE 512
#endif

#ifndef LED_RING_DIAL_FREERTOS_EVENT_QUEUE_LENGTH
    #define LED_RING_DIAL_FREERTOS_EVENT_QUEUE_LENGTH 8
#endif


#define LED_RING_LED_OFF     0,   0,   0
#define LED_RING_LED_RED     255, 0,   0
#define LED_RING_LED_GREEN   0,   255, 0
#define LED_RING_LED_BLUE    0,   0,   255
#define LED_RING_LED_TEST    128,   36,   99
#define LED_RING_LED_WHITE   255, 255, 255

struct LED_RING_LED_STATE {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
};

struct LED_RING_DIAL_STATE {
    LED_RING_LED_STATE LED;
    uint8_t POSITION;
};

/**
 * @class LedRing
 * @brief A class that represents an RGB I2C LED Ring
 */

 class LedRing {

    private:
    LEDRingSmall *Ring;
    static const int TOTAL_LEDS = 25;
    int m_startLedValue;

//    uint8_t m_LedColourState_red; // Used to keep track of the LED non-off state
//    uint8_t m_LedColourState_green;
//    uint8_t m_LedColourState_blue;

    // Array to store colour state for each LED
    LED_RING_LED_STATE m_ledColourStates[TOTAL_LEDS];


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
    LED_RING_LED_STATE getLedColourState(uint8_t logicalLEDPosition);

    // Change the colour state of a chosen logical LED position
    void setLedColourState(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b);

    /* FREE RTOS ASSOCIATED METHODS*/
    public:
    // Sets up the LED Ring dial task and creates the LED_Ring_Dial Queue
    void begin_dial();

    // Static Wrapper for vAddTask function
    static void vLedRingDialTask(void *pvParameters);

    // Main Task loop
    void processTaskLoop();

    void overwriteLedRingDialState(const LED_RING_DIAL_STATE& state)
    {
        xQueueOverwrite(m_ledRingDialQueue, &state);   // never blocks, always keeps the newest
    }

    private:
    volatile QueueHandle_t m_ledRingDialQueue = nullptr;
    volatile TaskHandle_t hdl_ledRingDialTask;
};