struct VOL_ENCODER_MIDI_STATE {
    
    VOL_ENCODER_MODE mode;
    uint8_t ledRing_Position;
    bool SW_pressEvent;
    uint8_t SW_numOfPresses;
    uint8_t SW_longPress;
    bool POT_changeEvent;
    uint8_t POT_value;
};

enum VOL_ENCODER_MODE 
{
    MODE1;
    MODE2;
    MODE3;
};
