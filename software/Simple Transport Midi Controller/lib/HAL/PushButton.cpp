#include "PushButton.h"
PushButton::PushButton(uint8_t pinAddr, uint32_t dbTime): m_pinAddr(pinAddr), m_dbTime(dbTime)
{
    m_time = millis();
    m_state = false;
    m_lastState = false;
    m_changed = false;
    m_lastChange = m_time;

    m_pressCount = 0;
    m_pressRead = false;
}

// Code adapted from JC_Button https://github.com/Kuantronic/JC_Button

bool PushButton::read(bool State) 
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

    m_time = ms;
    return m_state;
}

/*----------------------------------------------------------------------*
 * multiPressRead() checks the time between changes in the m_state      *
 * If it changes quick enough, it'll detect it as a multi presses       *
 * If a set time passes without another press, it'll return the times   *
 * it got multi-pressed
 *----------------------------------------------------------------------*/
uint8_t PushButton::multiPressRead(bool State) {

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
uint8_t PushButton::checkMultiPress() {

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
void PushButton::setMultiPressTimer(uint32_t multiPressTimeLimit) {
    m_multiPressTimeLimit = multiPressTimeLimit;
}

/*----------------------------------------------------------------------*
 * isPressed() and isReleased() check the button state when it was last *
 * read, and return false (0) or true (!=0) accordingly.                *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool PushButton::isPressed()
{
    return m_state;
}

bool PushButton::isReleased()
{
    return !m_state;
}

/*----------------------------------------------------------------------*
 * wasPressed() and wasReleased() check the button state to see if it   *
 * changed between the last two reads and return false (0) or           *
 * true (!=0) accordingly.                                              *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool PushButton::wasPressed()
{
    return m_state && m_changed;
}

bool PushButton::wasReleased()
{
    return !m_state && m_changed;
}

/*----------------------------------------------------------------------*
 * pressedFor(ms) and releasedFor(ms) check to see if the button is     *
 * pressed (or released), and has been in that state for the specified  *
 * time in milliseconds. Returns false (0) or true (!=0) accordingly.   *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool PushButton::pressedFor(uint32_t ms)
{
    return m_state && m_time - m_lastChange >= ms;
}

bool PushButton::releasedFor(uint32_t ms)
{
    return !m_state && m_time - m_lastChange >= ms;
}

/*----------------------------------------------------------------------*
 * lastChange() returns the time the button last changed state,         *
 * in milliseconds.                                                     *
 *----------------------------------------------------------------------*/
uint32_t PushButton::lastChange()
{
    return m_lastChange;
}