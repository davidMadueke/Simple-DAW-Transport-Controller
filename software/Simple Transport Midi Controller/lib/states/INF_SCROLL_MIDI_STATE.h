struct INF_SCROLL_MIDI_STATE
{
    uint8_t INF_SCROLL_MODE;
    bool SW_pressEvent;
    uint8_t SW_numOfPresses;
    uint8_t SW_longPress;
    uint8_t POT_value;
};

enum INF_SCROLL_MODE 
{
    MODE1;
    MODE2;
    MODE3;
}