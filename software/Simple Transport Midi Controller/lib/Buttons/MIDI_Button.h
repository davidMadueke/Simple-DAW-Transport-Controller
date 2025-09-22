#include "Arduino.h"
#include <Button.h>
#include <BUTTON_MIDI_STATE.h>

// Define the base class MIDI_Button
class MIDI_Button {
protected:
    Button* button;
    uint8_t m_LONG_PRESS_ms;
    BUTTON_LED_STATE* longPressLedState;
    BUTTON_LED_STATE* singlePressLedIndicator;

public:

    MIDI_Button(uint8_t i2cAddr, uint32_t dbTime, BUTTON_LED_STATE* longPressState, BUTTON_LED_STATE* singlePressState) {
        button = new Button(i2cAddr, dbTime);
        longPressLedState = longPressState;
        singlePressLedIndicator = singlePressState;
    }

    virtual ~MIDI_Button() {}

    virtual void begin(uint8_t brightnessPrescaler,
        uint8_t multiPressTimer = 150, uint8_t timeForLongPress = 1000) {
        setSinglePressLedIndicator(
            singlePressLedIndicator->Red,
            singlePressLedIndicator->Green,
            singlePressLedIndicator->Blue
        );
        setBrightness(brightnessPrescaler);
        button->setMultiPressTimer(multiPressTimer);
        setTimeForLongPress(timeForLongPress);
    }

    virtual void setBrightness(uint8_t brightnessPrescaler) { button->setBrightness(brightnessPrescaler); }

    virtual void setLedColour(uint8_t r, uint8_t g, uint8_t b) { button->setLedColour(r, g, b); }

    virtual void toggleLedOn() {
        setLedColour(singlePressLedIndicator);
    }

    virtual void setLedColour(BUTTON_LED_STATE* state) {
        setLedColour(
            state->Red,
            state->Green,
            state->Blue
        );
    }

    virtual void setSinglePressLedIndicator(uint8_t r, uint8_t g, uint8_t b) { button->setSinglePressLedIndicator(r, g, b); }

    virtual void setTimeForLongPress(uint8_t time_ms) { m_LONG_PRESS_ms = time_ms; }

    // Virtual stateMachine to be overridden
    virtual void stateMachine(bool ISR_State, BUTTON_HAL_STATE* HAL) = 0;
};