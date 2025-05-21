#pragma once

#include <DFRobot_RGBButton.h>
#include <Arduino.h>
#include "Wire.h"
#include "config/i2c_addresses.h"

/**
 * @class Button
 * @brief A class that represents a RGB I2C button
 */

 class Button {
    private:
    DFRobot_RGBButton RGBButton;
    uint8_t i2cAddr;

    

    public:
    typedef enum {
        eNoPress,
        eSinglePress,
        eDoublePress,
        eLongPress,
    } MIDI_OUT_MSG;
    void setLedColour(uint8_t r, uint8_t g, uint8_t b);
    void ledOff();
    bool getButtonStatus();
    void interruptServiceRoutine();

 };