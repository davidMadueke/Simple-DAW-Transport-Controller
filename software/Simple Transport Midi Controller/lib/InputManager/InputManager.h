#pragma once
#include <INF_SCROLL_MIDI_STATE.h>
#include <BUTTON_MIDI_STATE.h>
#include <VOL_ENCODER_MIDI_STATE.h>
#include <TransportManager.h>
#include <MIDI_PACKET.h>


class InputManager
{
    private:
    INF_SCROLL_MIDI_STATE* pInf_scroll = nullptr;
    BUTTON_MIDI_STATE* pButton_state = nullptr;
    VOL_ENCODER_MIDI_STATE* pVolume_state = nullptr;

    TransportManager* pTransport = nullptr;

    void (*pMIDIsendCallback)(MIDI_PACKET) = nullptr;

    public:
    InputManager(INF_SCROLL_MIDI_STATE* iState, BUTTON_MIDI_STATE* bState, VOL_ENCODER_MIDI_STATE* vState)
    : pInf_scroll(iState), pButton_state(bState), pVolume_state(vState) {};

    void attachTransportManager(TransportManager* tm) { 
        pTransport = tm;
    }
    void setMIDIsendCallback(void (*cb)(MIDI_PACKET)){ pMIDIsendCallback = cb; };
    
    void loop();


};