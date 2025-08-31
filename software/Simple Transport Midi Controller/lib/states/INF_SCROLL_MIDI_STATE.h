struct INF_SCROLL_MIDI_STATE
{
    INF_SCROLL_MODE MODE;
    bool SW_pressEvent;
    uint8_t SW_numOfPresses;
    uint8_t SW_longPress;
    uint8_t POT_value;
};

enum INF_SCROLL_MODE 
{
    ARRANGEMENT_ZOOM;
    PLAYBACK_ADJUST;
    RECORD_QUANTISATION;
};

struct INF_SCROLL_RGB_LED_STATE
{
    uint8_t RED;
    uint8_t GREEN;
    uint8_t BLUE;
}