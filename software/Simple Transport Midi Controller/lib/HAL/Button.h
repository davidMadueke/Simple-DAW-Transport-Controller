#include <DFRobot_RGBButton.h>
#include <Arduino.h>
#include <Wire.h>

/**
 * @class Button
 * @brief A class that represents a RGB I2C button
 */

 class Button {
    private:
    DFRobot_RGBButton *RGBButton;
    uint8_t m_i2cAddr;

    uint32_t m_dbTime;     // debounce time (ms)
    bool m_state;           // current button state, true=pressed
    bool m_lastState;       // previous button state
    bool m_changed;         // state changed since last read
    uint32_t m_time;        // time of current state (ms from millis)
    uint32_t m_lastChange;  // time of last state change (ms)
    
    uint8_t  m_pressCount;  // count the times button is pressed in rapid succession
    bool m_pressRead;       // bool status to make sure m_pressCount only increment once with one button press
    bool m_singlePressLedIndicator = false; // bool to check if to light an LED after every press
    uint8_t m_ledBrightnessPrescaler = 1;

    uint8_t m_ledIndicator_red; // Used in conjunction with when the button is pressed (not tracked by the non-off LED state manager)
    uint8_t m_ledIndicator_green;
    uint8_t m_ledIndicator_blue;

    uint8_t m_prevLedState_red; // Used to keep track of the LED non-off state
    uint8_t m_prevLedState_green;
    uint8_t m_prevLedState_blue;
    
    bool m_isLedOn; // Checks if there already has been a setLedColour call for use in single press led indicator logic
    
    uint32_t m_multiPressTimeLimit = 150; //time in ms it allows for next press to arrive to count as a multi-press
    uint8_t m_prevNumberOfPresses = 0;
    bool m_multiPressRead_longSinglePress = false; // A flag for when a single press exceeds the multi - press time limit


    //checks if the next press arrives
    //in the allowed time limit or not
    uint8_t checkMultiPress();

    public:
    Button(uint8_t i2cAddr, uint32_t dbTime);
    typedef enum {
        eNoPress,
        eSinglePress,
        eDoublePress,
        eLongPress,
    } MIDI_OUT_MSG;

    void setBrightness(uint8_t brightnessPrescaler);
    void setLedColour(uint8_t r, uint8_t g, uint8_t b);
    // Calls the setLedColour function with the previous LED state RGB values as arguments
    void prevLedState();
    void ledOn();
    void ledOff();
    
    bool getButtonStatus();

    void setSinglePressLedIndicator(uint8_t r, uint8_t g, uint8_t b);
    // Inspired by https://github.com/Kuantronic/JC_Button impl
    // Returns the current debounced button state, true for pressed,
        // false for released. Call this function frequently to ensure
        // the sketch is responsive to user input.
        bool read(bool State);

        // Returns true if the button state was pressed at the last call to read().
        // Does not cause the button to be read.
        bool isPressed();

        // Returns true if the button state was released at the last call to read().
        // Does not cause the button to be read.
        bool isReleased();

        // Returns true if the button state at the last call to read() was pressed,
        // and this was a change since the previous read.
        bool wasPressed();

        // Returns true if the button state at the last call to read() was released,
        // and this was a change since the previous read.
        bool wasReleased();

        // Returns true if the button state at the last call to read() was pressed,
        // and has been in that state for at least the given number of milliseconds.
        bool pressedFor(uint32_t ms);

        // Returns true if the button state at the last call to read() was released,
        // and has been in that state for at least the given number of milliseconds.
        bool releasedFor(uint32_t ms);

        // Returns the time in milliseconds (from millis) that the button last
        // changed state.
        uint32_t lastChange();

        // returns the number of presses done in rapid succession
        uint8_t multiPressRead(bool State);

        // overrides the time in ms it allows the next press to arrive to
        // count as a multi-press
        void setMultiPressTimer(uint32_t multiPressTimeLimit);
 };