#include <DFRobot_RGBButton.h>
#include <PushButton.h>
#include <Arduino.h>
#include <Wire.h>
#pragma once

/**
 * @class Button
 * @brief A class that represents a RGB I2C button
 */

 class Button : public PushButton {
    private:
    DFRobot_RGBButton *RGBButton;
    uint8_t m_i2cAddr;

    bool m_singlePressLedIndicator = false; // bool to check if to light an LED after every press
    uint8_t m_ledBrightnessPrescaler = 1;

    uint8_t m_ledIndicator_red; // Used in conjunction with when the button is pressed (not tracked by the non-off LED state manager)
    uint8_t m_ledIndicator_green;
    uint8_t m_ledIndicator_blue;

    uint8_t m_prevLedState_red; // Used to keep track of the LED non-off state
    uint8_t m_prevLedState_green;
    uint8_t m_prevLedState_blue;
    
    

    public:
    Button(const char* name, uint8_t i2cAddr, uint8_t interrupt_pin, uint32_t dbTime); 

    void begin(PushButtonDelivery delivery = PushButtonDelivery::Queue,
               QueueHandle_t eventQueue = nullptr,
               UBaseType_t queueLength = 4);
    
    void setBrightness(uint8_t brightnessPrescaler);
    void setLedColour(uint8_t r, uint8_t g, uint8_t b);
    
    // Calls the setLedColour function with the previous LED state RGB values as arguments
    void prevLedState();
    void ledOn();
    void ledOff();
    
    bool getButtonStatus();

    void setSinglePressLedIndicator(uint8_t r, uint8_t g, uint8_t b);
    void enableSinglePressIndicator(bool on);

    protected:
    void onStableStateApplied() override;
    virtual void ledSetup();

 };
