#include <Button.h>
#include "Arduino.h"
#include <BUTTON_MIDI_STATE.h>

class PlayButton
{
    private:

    Button* button;

    uint8_t m_LONG_PRESS_ms; //The time needed to hold the button down for a Long Press to be detected
    BUTTON_LED_STATE* longPressLedState;
    BUTTON_LED_STATE* singlePressLedIndicator;

    public:
    
    enum ButtonMode {
        SINGLE_PRESS,
        toLONG_PRESS,
        LONG_PRESS,
        toSINGLE_PRESS
    };

    PlayButton(uint8_t i2cAddr, uint32_t dbTime, BUTTON_LED_STATE* longPressState, BUTTON_LED_STATE* singlePressState){
        button = new Button(i2cAddr, dbTime);
        longPressLedState = longPressState;
        singlePressLedIndicator = singlePressState;
    };

    void begin(uint8_t brightnessPrescaler, 
        uint8_t multiPressTimer = 150, uint8_t timeForLongPress = 1000){
        setSinglePressLedIndicator(
            singlePressLedIndicator->Red,
            singlePressLedIndicator->Green,
            singlePressLedIndicator->Blue
        );

        setBrightness(brightnessPrescaler);
        button->setMultiPressTimer(multiPressTimer);
        setTimeForLongPress(timeForLongPress);
    };

     void setBrightness(uint8_t brightnessPrescaler) { button->setBrightness(brightnessPrescaler); };
    
     void setLedColour(uint8_t r, uint8_t g, uint8_t b){ button->setLedColour(r , g, b); };

    void toggleLedOn(){
        setLedColour(singlePressLedIndicator);
    };

    void setLedColour(BUTTON_LED_STATE* state){
        setLedColour(
            state->Red,
            state->Green,
            state->Blue
        );
    }

     void setSinglePressLedIndicator(uint8_t r, uint8_t g, uint8_t b){ button->setSinglePressLedIndicator(r,g,b); };

    void setTimeForLongPress(uint8_t time_ms){ m_LONG_PRESS_ms = time_ms; }

    // This function will handle what LEDState and ButtonState will be in or move to
    void stateMachine(bool ISR_State, BUTTON_HAL_STATE* HAL){
        static ButtonMode MODE;
        uint8_t numOfPresses = button->multiPressRead(ISR_State);
        switch (MODE){
            case SINGLE_PRESS:
                HAL->numOfPresses = numOfPresses;
                if (button->pressedFor(m_LONG_PRESS_ms)){
                    MODE = toLONG_PRESS;
                    HAL->longPress = true;
                    HAL->numOfPresses = 1;
                }
                break;
            // this is a transition state where we start the fast blink as feedback to the user,
        // but we also need to wait for the user to release the button, i.e. end the
        // long press, before moving to the LongPress state.
            case toLONG_PRESS:
                toggleLedOn();
                if (button->wasReleased())
                    MODE = LONG_PRESS;
                break;
            
            case LONG_PRESS:
                if(button->pressedFor(LONG_PRESS)){
                    HAL->longPress = false;
                    MODE = toSINGLE_PRESS;
                }
                else
                {
                    HAL->longPress = true;
                    HAL->numOfPresses = numOfPresses;
                    if(!button->isPressed()){
                        setLedColour(longPressLedState);
                    }
                }
                break;
            
            case toSINGLE_PRESS:
                if (button->wasReleased())
                    MODE = SINGLE_PRESS;
                break;
        }
    };

};
