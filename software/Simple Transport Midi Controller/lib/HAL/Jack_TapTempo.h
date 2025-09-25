#include <PushButton.h>
#include <TAP_TEMPO_STATE.h>
#include <Arduino.h>

class Jack_TapTempo
{
    private:
    PushButton* button;

    public:
    Jack_TapTempo(uint8_t pinAddr, uint32_t dbTime){
        button = new PushButton(pinAddr, dbTime);
    };

    bool read(uint8_t State) { return button->read(State); };

    // Method that returns true if pushbutton has been pressed
    bool wasPressed() { return button->wasPressed(); };

    // Method that takes as input a TAP TEMPO STATE and changes its value depending on if the tap tempo switch has been pressed
    void polling(TAP_TEMPO_STATE* TEMPO){
        if (wasPressed())
        {
            TEMPO->tapTempoEvent = true; //asserting this flag off will need to be handled someplace else
        }
    };

};