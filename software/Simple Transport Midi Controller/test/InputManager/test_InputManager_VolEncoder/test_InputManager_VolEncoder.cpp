#include <Arduino.h>
#include <unity.h>
#include <freertos/queue.h>
#include <Wire.h>
#include <InputManager.h>
#include <InputSource.h>
#include <VolumeEncoder.h>
#include <MIDI_PACKET.h>
#include <DisplayAction.h>
#include <i2c_addresses.h>
#include <constants.h>
#include <rSerial.h>

#define PIN_IN1 A2
#define PIN_IN2 A3
#define PIN_BTN A4
#define TEST_LED_RING_START_VALUE 7U

#ifndef TEST_INPUT_MANAGER_CONSUMER_STACK_SIZE
    #define TEST_INPUT_MANAGER_CONSUMER_STACK_SIZE 4096
#endif

#ifndef TEST_INPUT_MANAGER_CONSUMER_PRIORITY
    #define TEST_INPUT_MANAGER_CONSUMER_PRIORITY 2
#endif

VolumeEncoder* testEncoder = new VolumeEncoder(
    PIN_IN1,
    PIN_IN2,
    PIN_BTN,
    50,
    I2C_ADDRESS_LED_RING,
    INPUT_PULLUP
);

static InputManager* manager = nullptr;

void setUp(void) {}
void tearDown(void) {}

static const char* displayActionTypeToString(DISPLAY_ACTION::TYPE type)
{
    switch (type)
    {
        case DISPLAY_ACTION::TYPE::VOL_ENCODER_MODE_INCREMENTED: return "VOL_ENCODER_MODE_INCREMENTED";
        case DISPLAY_ACTION::TYPE::VOL_ENCODER_POSITION: return "VOL_ENCODER_POSITION";
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
            if(action.type == DISPLAY_ACTION::TYPE::VOL_ENCODER_MODE_INCREMENTED)
            {
                rSerial.printf(
                "[DISPLAY] type=%s volMode=%u\n",
                displayActionTypeToString(action.type),
                static_cast<unsigned>(action.vol_encoder_mode)
                );
            }
            else if (action.type == DISPLAY_ACTION::TYPE::VOL_ENCODER_POSITION)
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
}

void setup()
{
    UNITY_BEGIN();
    rSerial.begin(115200);

    Wire.begin();
    Wire.setClock(400000);

    testEncoder->begin(TEST_LED_RING_START_VALUE);

    manager = new InputManager();
    manager->registerInputSource(
        InputSource{testEncoder->getVolEncoderEventQueueHandle(), InputSource::Type::VolEncoder}
    );
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
