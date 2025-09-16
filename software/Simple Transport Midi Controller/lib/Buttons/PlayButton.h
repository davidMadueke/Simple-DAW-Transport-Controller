#include <Button.h>
#include "Arduino.h"

class PlayButton
{
    private:

    Button* button;

    public:
    
    enum ButtonMode {
        SINGLE_PRESS,
        toLONG_PRESS,
        LONG_PRESS,
        toSINGLE_PRESS
    };

    PlayButton(uint8_t i2cAddr, uint32_t dbTime){
        button = new Button(i2cAddr, dbTime);
    };

     void setBrightness(uint8_t brightnessPrescaler) { button->setBrightness(); };
    
     void setLedColour(uint8_t r, uint8_t g, uint8_t b){ button->setLedColour(); };

    void setSinglePressLedIndicator(uint8_t r, uint8_t g, uint8_t b){ button->setSinglePressLedIndicator(r,g,b); };

};
