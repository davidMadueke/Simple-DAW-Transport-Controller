#pragma once
#include <PushButton.h>
//#include <TAP_TEMPO_STATE.h>
#include <Arduino.h>

class Jack_TapTempo
{
private:
    PushButton* button;

public:
    Jack_TapTempo(uint8_t pinAddr, uint32_t dbTime)
    {
        button = new PushButton("TapTempo", pinAddr, dbTime);
    }

    void begin()
    {
        button->setButtonMode(INPUT);
        button->begin(PushButtonDelivery::Queue, nullptr, 8);
    }

    void attachLedWriteCallback(LedWriteCallback callback){
        button->attachLedWriteCallback(callback);
    };

    void addInducedTapTempoEventToQueue(PushButtonEvent::Type event){
        button->postEventPublic(event);
        #ifdef METRONOME_BUTTON_DEBUG
            Serial.printf("Posting Press/Release Metronome Event as TAP TEMPO %u\n", static_cast<unsigned>(event));
        #endif
    }

    bool consumePressEdge(){
        return button->consumePressEdge();
    }

    bool consumeReleaseEdge(){
        return button->consumeReleaseEdge();
    }
    QueueHandle_t getButtonEventQueueHandle() { return button->getButtonEventQueueHandle();}
};
