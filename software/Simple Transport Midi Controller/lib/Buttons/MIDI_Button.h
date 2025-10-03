#include "Arduino.h"
#include <Button.h>
#include <BUTTON_MIDI_STATE.h>

// Define the base class MIDI_Button
class MIDI_Button {
protected:
    uint8_t m_LONG_PRESS_ms;

    // Setup stored LED States struct for different types of buttons to be setup
    BUTTON_LED_STATE* longPressLedState;
    BUTTON_LED_STATE* singlePressLedIndicator;
    BUTTON_LED_STATE* toggleLedState;
    bool m_noLedIndicator;

public:
    Button* button;
    MIDI_Button(uint8_t i2cAddr, uint32_t dbTime) {
        button = new Button(i2cAddr, dbTime);
        m_noLedIndicator = true;
        
    }

    virtual ~MIDI_Button() {}

    virtual void begin(uint8_t brightnessPrescaler,
        uint8_t multiPressTimer = 150) {
        setBrightness(brightnessPrescaler);
        button->setMultiPressTimer(multiPressTimer);
    }

    // Setup an SinglePressLedIndicator begin
    virtual void setup_singlePressLedIndicator(BUTTON_LED_STATE* singlePressState){
        m_noLedIndicator = false;
        singlePressLedIndicator = singlePressState;
        setSinglePressLedIndicator(
            singlePressLedIndicator->Red,
            singlePressLedIndicator->Green,
            singlePressLedIndicator->Blue
        );

    }

    // Setup a LongPressLedState begin
    virtual void setup_longPress(BUTTON_LED_STATE* longPressState, uint8_t timeForLongPress = 1000 ){
        setTimeForLongPress(timeForLongPress);
        longPressLedState = longPressState;
    }

    virtual void setup_toggleLedState(BUTTON_LED_STATE* toggleLedState){
        toggleLedState = toggleLedState;
    }

    virtual void setBrightness(uint8_t brightnessPrescaler) { button->setBrightness(brightnessPrescaler); }

    virtual void setLedColour(uint8_t r, uint8_t g, uint8_t b) { button->setLedColour(r, g, b); }

    virtual void toggleLedOn() {
        setLedColour(toggleLedState);
    }

    virtual void ledOff(){ button->ledOff(); }

    virtual void setLedColour(BUTTON_LED_STATE* state) {
        setLedColour(
            state->Red,
            state->Green,
            state->Blue
        );
    }

    virtual void setSinglePressLedIndicator(uint8_t r, uint8_t g, uint8_t b) { 
        if(!m_noLedIndicator){
            button->setSinglePressLedIndicator(r, g, b); 
        }
    }

    virtual void setTimeForLongPress(uint8_t time_ms) { m_LONG_PRESS_ms = time_ms; };

    virtual uint8_t getTimeForLongPress(){ return m_LONG_PRESS_ms; };

    // Virtual stateMachine to be overridden
    virtual void stateMachine(bool ISR_State, BUTTON_HAL_STATE* HAL) = 0;
};