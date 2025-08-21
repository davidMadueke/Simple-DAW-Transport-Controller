#include "SysPowerSwitch.h"

SysPowerSwitch::SysPowerSwitch(uint8_t pin)
{
    m_pin_number = pin;
}
bool SysPowerSwitch::read(bool State)
{
    m_state = State;

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