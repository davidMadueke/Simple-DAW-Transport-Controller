#include <SysPowerSwitch.h>
#include <Arduino.h>


#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)  // 2 ^ GPIO_NUMBER in hex, used for setting up ext1 WakeUp 

class PowerManager_ESP32
{
    private:
    SysPowerSwitch *pwr_switch;

    TaskHandle_t hdl_sysPwrTask = nullptr;

    public:
    /* Requires a defined GPIO pin for a HW power switch. Note that only RTC IO can be used as a source for external wake
  source on the ESP 32.*/
    PowerManager_ESP32(uint8_t pin_pwr_switch, uint32_t db_time = 500U);

    // The FreeRTOS callback attachd to the FreeRTOS SysPower Task
    static void vSysPwrTask(void* pvParameters);
    void processTaskLoop();

    // Initialises the toggle button and sets up its associated FreeRTOS task
    void begin();
    
    // Method that when called sends the ESP32 to light sleep mode. CPU is paused in action, WI-FI and BT switched off
    void lightSleep();
    
    // Method that when called sends the ESP32 to deep sleep mode. 0.1 uA power consumption. CPU is turned off, RAM turned off, WI-FI and BT switched off
    // Only RTC Peripherals are operational
    void sysTurnOff();

    void sysPowerOn();

    /* This function is called in the main running setup() of the codebase. 
    It sets up the initialised pwr_switch object to be the GPIO ext1 external wakeUp Source - such that when this switch is asserted high
    the system will escape from deep sleep - after sysTurnOff() has been called at runtime
    */
    void setup_pwrSwitch_as_extWakeUp();

    uint8_t getPin();
};