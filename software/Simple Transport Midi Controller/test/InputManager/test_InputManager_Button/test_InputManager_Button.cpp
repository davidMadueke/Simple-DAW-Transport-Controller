#include <Arduino.h>
#include <unity.h>
#include <freertos/queue.h>
#include <InputManager.h>
#include <InputSource.h>
#include <BUTTON_MIDI_STATE.h>
#include <QuantButton.h>
#include <MIDI_PACKET.h>
#include <Wire.h>
#include <i2c_addresses.h>
#include <DisplayAction.h>
#include <constants.h>
#include <rSerial.h>

#define TEST_BTN_I2C_ADDR I2C_BTN_METRONOME_INT
#define TEST_BTN_INT_PIN 6

#ifndef TEST_INPUT_MANAGER_CONSUMER_STACK_SIZE
    #define TEST_INPUT_MANAGER_CONSUMER_STACK_SIZE 4096
#endif

#ifndef TEST_INPUT_MANAGER_CONSUMER_PRIORITY
    #define TEST_INPUT_MANAGER_CONSUMER_PRIORITY 2
#endif

BUTTON_LED_STATE singlePressState = {128, 128, 128};

QuantButton* testButton = new QuantButton(
    TEST_BTN_I2C_ADDR, TEST_BTN_INT_PIN,
    BUTTON_DEBOUNCE_TIME_MS,
    &singlePressState);

static InputManager* manager = nullptr;

void setUp(void) {}
void tearDown(void) {}

static const char* displayActionTypeToString(DISPLAY_ACTION::TYPE type)
{
    switch (type)
    {
        case DISPLAY_ACTION::TYPE::TAP_TEMPO: return "TAP_TEMPO";
        case DISPLAY_ACTION::TYPE::DAW_TEMPO_CHANGED: return "DAW_TEMPO_CHANGED";
        case DISPLAY_ACTION::TYPE::BATTERY_LOW_PWR: return "BATTERY_LOW_PWR";
        case DISPLAY_ACTION::TYPE::INF_SCROLL_MODE_INCREMENTED: return "INF_SCROLL_MODE_INCREMENTED";
        case DISPLAY_ACTION::TYPE::VOL_ENCODER_MODE_INCREMENTED: return "VOL_ENCODER_MODE_INCREMENTED";
        case DISPLAY_ACTION::TYPE::BLE_CONNECTED: return "BLE_CONNECTED";
        case DISPLAY_ACTION::TYPE::BUTTON_PRESS: return "BUTTON_PRESS";
        default: return "UNKNOWN";
    }
}

static void vMidiConsumerTask(void* pvParameters)
{
    InputManager* mgr = static_cast<InputManager*>(pvParameters);
    MIDI_PACKET packet;

    for (;;)
    {
        if (xQueueReceive(mgr->getMidiQueue(), &packet, portMAX_DELAY) == pdTRUE)
        {
            rSerial.printf(
                "[MIDI] type=%u CC=%u value=%u ch=%u\n",
                static_cast<unsigned>(packet.type),
                packet.data1,
                packet.data2,
                packet.channel
            );
        }
    }
}

static void vDisplayConsumerTask(void* pvParameters)
{
    InputManager* mgr = static_cast<InputManager*>(pvParameters);
    DISPLAY_ACTION action;

    for (;;)
    {
        if (xQueueReceive(mgr->getDisplayQueue(), &action, portMAX_DELAY) == pdTRUE)
        {
            rSerial.printf(
                "[DISPLAY] type=%s midiCC=%u midiValue=%u\n",
                displayActionTypeToString(action.type),
                action.midi_packet.data1,
                action.midi_packet.data2
            );
        }
    }
}

void setup()
{
    UNITY_BEGIN();
    rSerial.begin(115200);

    testButton->begin(10);

    manager = new InputManager();
    manager->registerInputSource(InputSource{testButton->getEventQueueHandle(), InputSource::Type::RgbButton});
    manager->begin();

    xTaskCreate(
        vMidiConsumerTask,
        "midi_cons",
        TEST_INPUT_MANAGER_CONSUMER_STACK_SIZE,
        manager,
        TEST_INPUT_MANAGER_CONSUMER_PRIORITY,
        nullptr
    );

    xTaskCreate(
        vDisplayConsumerTask,
        "disp_cons",
        TEST_INPUT_MANAGER_CONSUMER_STACK_SIZE,
        manager,
        TEST_INPUT_MANAGER_CONSUMER_PRIORITY,
        nullptr
    );
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
