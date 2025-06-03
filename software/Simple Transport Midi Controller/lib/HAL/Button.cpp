
#include "Button.h"

Button::Button(uint8_t i2cAddr, uint32_t dbTime): m_i2cAddr(i2cAddr), m_dbTime(dbTime)
{
    m_i2cAddr = i2cAddr;
    RGBButton = new DFRobot_RGBButton(&Wire, i2cAddr);

    if (!initFailed())
    {
        m_time = millis();
        m_state = false;
        m_lastState = false;
        m_changed = false;
        m_lastChange = m_time;

        m_pressCount = 0;
        m_pressRead = false;
    }
}

bool Button::initFailed()
{return !RGBButton->begin();}

void Button::setLedColour(uint8_t r, uint8_t g, uint8_t b)
{
    RGBButton->setRGBColor(r, g, b);
}

void Button::ledOff()
{
    RGBButton->setRGBColor(0, 0, 0);
}

bool Button::getButtonStatus()
{
    return RGBButton->getButtonStatus();
}

// Code adapted from JC_Button https://github.com/Kuantronic/JC_Button
// To use the I2C bus for reading Button state instead

bool Button::read()
{
    uint32_t ms = millis();
    if (ms - m_lastChange < m_dbTime)
    {
        m_changed = false;
    }
    else
    {
        m_lastState = m_state;
        m_state = getButtonStatus();
        m_changed = (m_state != m_lastState);
        if (m_changed) m_lastChange = ms;
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
uint8_t Button::multiPressRead() {

    if (read()) {
        //makes sure it counts the press ONLY once
        if (!m_pressRead) {
            m_pressCount++;
            m_pressRead = true;
        }
        return checkMultiPress();
    }

    if (!read()) {
        m_pressRead = false;
        return checkMultiPress();
    }

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
        return numberOfPresses;
    }
    else {
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