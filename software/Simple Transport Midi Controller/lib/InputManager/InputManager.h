#pragma once
#include <INF_SCROLL_MIDI_STATE.h>
#include <BUTTON_MIDI_STATE.h>
#include <VOL_ENCODER_MIDI_STATE.h>
#include <TransportManager.h>
#include <MIDI_PACKET.h>
#include <InputCompiler.h>

#ifndef INPUT_MANAGER_MAX_INPUT_SOURCES
    #define INPUT_MANAGER_MAX_INPUT_SOURCES 16u
#endif

#ifndef INPUT_MANAGER_FREERTOS_PRIORITY
    #define INPUT_MANAGER_FREERTOS_PRIORITY 5
#endif
#ifndef INPUT_MANAGER_FREERTOS_TASK_STACK_SIZE
    #define INPUT_MANAGER_FREERTOS_TASK_STACK_SIZE 4096
#endif


struct InputSource
{
    QueueHandle_t queue;
    enum class Type { RgbButton, TapTempo, Encoder } type;
};

class InputManager
{
    private:

    size_t m_queueSetLength = 0;
    TaskHandle_t hdl_InputMgrTask = nullptr;
    QueueSetHandle_t m_inputSet = nullptr;


    static constexpr size_t kMaxInputs = INPUT_MANAGER_MAX_INPUT_SOURCES;
    InputSource m_inputs[kMaxInputs] = {};
    size_t m_inputCount = 0;

    INF_SCROLL_MIDI_STATE* pInf_scroll = nullptr;
    BUTTON_MIDI_STATE* pButton_state = nullptr;
    VOL_ENCODER_MIDI_STATE* pVolume_state = nullptr;

    TransportManager* pTransport = nullptr;

    void (*pMIDIsendCallback)(MIDI_PACKET) = nullptr;

    void inputManagerTaskLoop();

    public:
    InputManager(INF_SCROLL_MIDI_STATE* iState, BUTTON_MIDI_STATE* bState, VOL_ENCODER_MIDI_STATE* vState)
    : pInf_scroll(iState), pButton_state(bState), pVolume_state(vState) {};

    void begin();

    static void vInputManagerTask(void* pvParameters);


    void registerInputSource(InputSource input);

    void attachTransportManager(TransportManager* tm) { 
        pTransport = tm;
    }
    void setMIDIsendCallback(void (*cb)(MIDI_PACKET)){ pMIDIsendCallback = cb; };
    
    void loop();


};