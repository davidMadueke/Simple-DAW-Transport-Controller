#include <Arduino.h>
#include <PushButton.h>
/**
 * @class SysPowerSwitch
 * @brief A class that represents the HW Button that controls System Power
 */

 class SysPowerSwitch
 {

    private:
    ToggleSwitch* button;
    uint8_t _pin;
    uint32_t _dbTime;

public:
    SysPowerSwitch(uint8_t pin, uint32_t dbTime = 500 /*ms*/) : _pin(pin), _dbTime(dbTime)
    {
        button = new ToggleSwitch("PWR", pin, true, dbTime);
    }
    
    void begin()
    {
        button->begin(PushButtonDelivery::Polling);
    }

    ToggleSwitch* getButton() { return button;};

    // Returns true if the power button has been switched off.
    // Does not cause the button to be read.
    bool getToggleState() { return button->toggleState();};

    uint8_t getPin() {return _pin;};

 };