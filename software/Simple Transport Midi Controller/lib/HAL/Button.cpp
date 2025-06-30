
#include "Button.h"

Button::Button(uint8_t i2cAddr, uint32_t dbTime): m_i2cAddr(i2cAddr), m_dbTime(dbTime)
{
    m_i2cAddr = i2cAddr;
    RGBButton = new DFRobot_RGBButton(&Wire1, i2cAddr);

    m_time = millis();
    m_state = false;
    m_lastState = false;
    m_changed = false;
    m_lastChange = m_time;

    m_pressCount = 0;
    m_pressRead = false;
}

void Button::setLedColour(uint8_t r, uint8_t g, uint8_t b)
{
    //set the previous non-off LED state to this colour
    m_prevLedState_red = r;
    m_prevLedState_green = g;
    m_prevLedState_blue = b;
    
    r /= m_ledBrightnessPrescaler;
    g /= m_ledBrightnessPrescaler;
    b /= m_ledBrightnessPrescaler;

    RGBButton->setRGBColor(r, g, b);
    
}

void Button::setBrightness(uint8_t brightnessPrescaler)
{
    m_ledBrightnessPrescaler = brightnessPrescaler;
}

void Button::prevLedState()
{
    setLedColour(m_prevLedState_red, m_prevLedState_green, m_prevLedState_blue);
}

void Button::ledOn()
{
    RGBButton->setRGBColor(m_ledIndicator_red, m_ledIndicator_green, m_ledIndicator_blue);
}

void Button::ledOff()
{
    // Use Class member function override such that the system remembers a previous off LED state
    setLedColour(0, 0, 0);
}

bool Button::getButtonStatus()
{
    return RGBButton->getButtonStatus();
}

void Button::setSinglePressLedIndicator(uint8_t r, uint8_t g, uint8_t b)
{
       /**
     * @brief Set the member flag so to ensure to light an LED after every press
     * @param r - PWM red channel for the single press colour
     * @param g - PWM green channel for the single press colour
     * @param b - PWM blue channel for the single press colour
     * @return None
     */
    m_singlePressLedIndicator = true;
    m_ledIndicator_red = r;
    m_ledIndicator_green = g;
    m_ledIndicator_blue = b;
}
// Code adapted from JC_Button https://github.com/Kuantronic/JC_Button
// To use the rgb button interrupt for reading Button state instead

bool Button::read(bool State) 
{
    uint32_t ms = millis();
    if (ms - m_lastChange < m_dbTime)
    {
        m_changed = false;
    }
    else
    {
        m_lastState = m_state;
        m_state = State;
        m_changed = (m_state != m_lastState);
        if (m_changed) 
        {
            m_lastChange = ms;
            
        }
    }

    if (isPressed()) {
        //  
        //if(!pressedFor(5))
        ledOn();
        Serial.println("Is Pressed");
    } 
    else {
        if(wasReleased())
        {
            // check whether previous LED state was not off
            // if so then revert to the previous LED state
            // if not then turn the LED off
            if (
                (m_prevLedState_red == 0) &&
                (m_prevLedState_green == 0) &&
                (m_prevLedState_blue == 0)
            ) {
                Serial.println("led off");
                ledOff();
            } else {
                Serial.println("Prev LED state");
                prevLedState();
            } 
            Serial.println("Is not Pressed");
        } 
    } 
    m_time = ms;
    return m_state;
}

/*----------------------------------------------------------------------*
 * multiPressRead() checks the time between changes in the m_state      *
 * If it changes quick enough, it'll detect it as a multi presses       *
 * If a set time passes without another press, it'll return the times   *
 * it got multi-pressed
 *----------------------------------------------------------------------*/
uint8_t Button::multiPressRead(bool State) {

    if(m_multiPressRead_longSinglePress && wasReleased())
        {
            Serial.println("asserting MPRLSP2 off");
            m_multiPressRead_longSinglePress = false;
            return 1;
        }
    // Makes a note of whether a single press has exceed the multipress time limit
    if(pressedFor(m_multiPressTimeLimit)) 
    {
        Serial.println("asserting MPRLSP on");
        m_multiPressRead_longSinglePress = true;
    }
    
    bool readState = read(State);
    if (readState) {
        //makes sure it counts the press ONLY once
        if (!m_pressRead) {
            m_pressCount++;
            m_pressRead = true;
        }
        return checkMultiPress();
    }

    if (!readState) {
        m_pressRead = false;
        return checkMultiPress();
    }

    
        //else {return 0;}
        return 0;
}

/*----------------------------------------------------------------------*
 * Checks the time that passed without another press. If it exceeds the *
 * limit, it returns the amount of fast presses it counted.             *
 *----------------------------------------------------------------------*/
uint8_t Button::checkMultiPress() {

    if (millis() - m_lastChange > m_multiPressTimeLimit) {
        uint8_t numberOfPresses = m_pressCount;
        m_pressCount = 0;
        m_prevNumberOfPresses = numberOfPresses;
        
        return numberOfPresses;
    }
    else {
        
        Serial.println("returning zero");
        return 0;
    }
}

/*----------------------------------------------------------------------*
 * Sets the time in ms for the next button press to be counted as       *
 * a successive multi press.                                            *
 *----------------------------------------------------------------------*/
void Button::setMultiPressTimer(uint32_t multiPressTimeLimit) {
    m_multiPressTimeLimit = multiPressTimeLimit;
}

/*----------------------------------------------------------------------*
 * isPressed() and isReleased() check the button state when it was last *
 * read, and return false (0) or true (!=0) accordingly.                *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Button::isPressed()
{
    return m_state;
}

bool Button::isReleased()
{
    return !m_state;
}

/*----------------------------------------------------------------------*
 * wasPressed() and wasReleased() check the button state to see if it   *
 * changed between the last two reads and return false (0) or           *
 * true (!=0) accordingly.                                              *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Button::wasPressed()
{
    return m_state && m_changed;
}

bool Button::wasReleased()
{
    return !m_state && m_changed;
}

/*----------------------------------------------------------------------*
 * pressedFor(ms) and releasedFor(ms) check to see if the button is     *
 * pressed (or released), and has been in that state for the specified  *
 * time in milliseconds. Returns false (0) or true (!=0) accordingly.   *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Button::pressedFor(uint32_t ms)
{
    return m_state && m_time - m_lastChange >= ms;
}

bool Button::releasedFor(uint32_t ms)
{
    return !m_state && m_time - m_lastChange >= ms;
}

/*----------------------------------------------------------------------*
 * lastChange() returns the time the button last changed state,         *
 * in milliseconds.                                                     *
 *----------------------------------------------------------------------*/
uint32_t Button::lastChange()
{
    return m_lastChange;
}