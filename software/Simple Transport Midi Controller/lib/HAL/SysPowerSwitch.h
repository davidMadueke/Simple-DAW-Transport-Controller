#include <Arduino.h>
/**
 * @class SysPowerSwitch
 * @brief A class that represents the HW Button that controls System Power
 */

 class SysPowerSwitch
 {
    private:
    uint8_t m_pin_number;
   
    uint8_t m_dbTime; // debounce time (ms)
    uint8_t m_state;

    bool m_lastState;       // previous button state
    bool m_changed;         // state changed since last read
    uint32_t m_time;        // time of current state (ms from millis)
    uint32_t m_lastChange;  // time of last state change (ms)

    public:
    SysPowerSwitch(uint8_t pin, uint32_t dbTime = 500 /*ms*/);

    // Reads a boolean - presumably from a ISR variable
    bool read(bool State);

    // Returns true if the button state was changed to On.
    // Does not cause the button to be read.
    bool isSwitchedOn();

    uint8_t getPin();

 };