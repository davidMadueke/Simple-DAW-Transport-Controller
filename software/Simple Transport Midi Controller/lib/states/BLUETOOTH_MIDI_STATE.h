#ifndef BLUETOOTH_MIDI_STATE_H
#define BLUETOOTH_MIDI_STATE_H

struct BLUETOOTH_MIDI_STATE
{
    bool enable;
    bool scanning; // if true start advertising till a connection is made
    bool isInitialised;
    bool midi_Rx;
    bool midi_Tx;
};

#endif // BLUETOOTH_MIDI_STATE_H