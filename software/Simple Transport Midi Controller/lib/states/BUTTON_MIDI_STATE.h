struct BUTTON_MIDI_STATE
{
    BUTTON_HAL_STATE playButton;
    BUTTON_HAL_STATE pauseButton;
    BUTTON_HAL_STATE recButton;
    BUTTON_HAL_STATE overdubButton;
    BUTTON_HAL_STATE quantiseButton;
    BUTTON_HAL_STATE metronomeButton;
    bool tapTempoButton_PressEvent; // This will be used for both the when the metronome BTN is in Tap Tempo mode and the Tap Tempo footswitch Input
};

struct BUTTON_HAL_STATE
{
    bool buttonPressEvent;
    uint8_t numOfPresses;
    uint8_t longPress;
}