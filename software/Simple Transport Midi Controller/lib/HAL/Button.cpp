
#include "Button.h"
Button::Button(const char* name, uint8_t i2cAddr, uint8_t interrupt_pin, uint32_t dbTime): 
    m_i2cAddr(i2cAddr), PushButton(name, interrupt_pin, dbTime, true)
{
    RGBButton = new DFRobot_RGBButton(&Wire, i2cAddr);
}

void Button::begin(PushButtonDelivery delivery,
                         QueueHandle_t eventQueue,
                         UBaseType_t queueLength)
{
    while( ! RGBButton->begin() ){
    Serial.println("Communication with device failed, please check connection!");
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
    attachDigitalReadCallback([this]() { return getButtonStatus(); });
    vSetTaskStackSize(8192U);
    PushButton::begin(delivery, eventQueue, queueLength);
}

void Button::ledSetup() {
    
}

void Button::onStableStateApplied() 
{
    if(!m_singlePressLedIndicator){ return;}
    if(wasPressed()) { ledOn();}
    else if (wasReleased()){
        // check whether previous LED state was not off
        // if so then revert to the previous LED state
        // if not then turn the LED off
        if (
            (m_prevLedState_red == 0) &&
            (m_prevLedState_green == 0) &&
            (m_prevLedState_blue == 0)
        ) {
            //Serial.println("led off");
            ledOff();
        } else {
            //Serial.println("Prev LED state");
            prevLedState();
        } 
    }
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
