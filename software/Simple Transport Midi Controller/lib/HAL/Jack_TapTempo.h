#include <PushButton.h>
#include <TAP_TEMPO_STATE.h>
#include <Arduino.h>

class Jack_TapTempo
{
private:
    PushButton* button;

public:
    Jack_TapTempo(uint8_t pinAddr, uint32_t dbTime)
    {
        button = new PushButton(pinAddr, dbTime);
    }

    void begin()
    {
        button->begin(PushButtonDelivery::Polling);
    }

    void polling(TAP_TEMPO_STATE* TEMPO)
    {
        if (button->consumePressEdge()) {
            TEMPO->tapTempoEvent = true;
        }
    }
};
