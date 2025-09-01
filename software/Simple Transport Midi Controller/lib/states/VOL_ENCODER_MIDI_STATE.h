struct VOL_ENCODER_MIDI_STATE {
    
    VOL_ENCODER_MODE mode;
    VOL_ENCODER_RGB_LED_RING_STATE ledRingState;
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

struct VOL_ENCODER_RGB_LED_RING_STATE
{
    uint8_t COLOUR_RED;
    uint8_t COLOUR_GREEN;
    uint8_t COLOUR_BLUE;
    uint8_t POSITION
}
