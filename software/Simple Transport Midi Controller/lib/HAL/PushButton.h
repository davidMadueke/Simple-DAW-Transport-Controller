#include <Arduino.h>
#include <Wire.h>
#pragma once
/**
 * @class Button
 * @brief A class that represents a general push button
 */

 class PushButton {
    private:
    uint8_t m_pinAddr;

    uint32_t m_dbTime;     // debounce time (ms)
    bool m_state;           // current button state, true=pressed
    bool m_lastState;       // previous button state
    bool m_changed;         // state changed since last read
    uint32_t m_time;        // time of current state (ms from millis)
    uint32_t m_lastChange;  // time of last state change (ms)
    
    uint8_t  m_pressCount;  // count the times button is pressed in rapid succession
    bool m_pressRead;       // bool status to make sure m_pressCount only increment once with one button press
    
    uint32_t m_multiPressTimeLimit = 150; //time in ms it allows for next press to arrive to count as a multi-press
    uint8_t m_prevNumberOfPresses = 0;
    bool m_multiPressRead_longSinglePress = false; // A flag for when a single press exceeds the multi - press time limit


    //checks if the next press arrives
    //in the allowed time limit or not
    uint8_t checkMultiPress();

    public:
    PushButton(uint8_t pinAddr, uint32_t dbTime);
    PushButton(uint32_t dbTime);

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

 // a derived class for a "push-on, push-off" (toggle) type button.
// initial state can be given, default is off (false).
// inspired by JChristensen's Impl https://github.com/JChristensen/JC_Button
class ToggleSwitch : public PushButton
{
    public:

        // constructor is similar to Button, but includes the initial state for the toggle.
        ToggleSwitch(uint8_t pin, bool initialState=false, uint32_t dbTime=25)
            : PushButton(pin, dbTime), m_toggleState(initialState) {}

        // read the button and return its state.
        // should be called frequently.
        bool read(bool State)
        {
            PushButton::read(State);
            if (wasPressed()) {
                m_toggleState = !m_toggleState;
                m_changed = true;
            }
            else {
                m_changed = false;
            }
            return m_toggleState;
        }

        // has the state changed?
        bool changed() const {return m_changed;}

        // return the current state
        bool toggleState() const {return m_toggleState;}

    private:
        bool m_toggleState;
        bool m_changed = false;
};