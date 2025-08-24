#include "SysPowerSwitch.h"

SysPowerSwitch::SysPowerSwitch(uint8_t pin, uint32_t dbTime): m_dbTime(dbTime)
{
    m_pin_number = pin;
    m_lastState = false;
    m_changed = false;
    m_lastChange = m_time;
}
bool SysPowerSwitch::read(bool State)
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

bool SysPowerSwitch::isSwitchedOn()
{
    return m_state;
}

uint8_t SysPowerSwitch::getPin()
{
    return m_pin_number;
}