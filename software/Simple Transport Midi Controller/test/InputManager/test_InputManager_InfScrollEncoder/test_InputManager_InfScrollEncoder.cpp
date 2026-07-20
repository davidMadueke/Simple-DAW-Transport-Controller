#include <Arduino.h>
#include <unity.h>
#include <freertos/queue.h>
#include <Wire.h>
#include <InputManager.h>
#include <InputSource.h>
#include <infScrollEncoder.h>
#include <SparkFunSX1509.h>
#include <MIDI_PACKET.h>
#include <DisplayAction.h>
#include <constants.h>
#include <rSerial.h>

#define PIN_IN1 A2
#define PIN_IN2 A3
#define PIN_BTN A4
#define PIN_LED_R_SX1509 0
#define PIN_LED_G_SX1509 1
#define PIN_LED_B_SX1509 2

#ifndef TEST_INPUT_MANAGER_CONSUMER_STACK_SIZE
    #define TEST_INPUT_MANAGER_CONSUMER_STACK_SIZE 4096
#endif

#ifndef TEST_INPUT_MANAGER_CONSUMER_PRIORITY
    #define TEST_INPUT_MANAGER_CONSUMER_PRIORITY 2
#endif

byte SX1509_ADDRESS = 0x3E;
SX1509 io;

InfScrollEncoder* testEncoder = nullptr;
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

    Wire1.setPins(SDA, SCL);
    pinMode(SDA, OUTPUT);
    digitalWrite(SDA, HIGH);
    Wire1.begin();

    if (io.begin(SX1509_ADDRESS, Wire1) == false)
    {
        rSerial.println("Failed to communicate with SX1509.");
        while (1) { ; }
    }

    // Duppa RGB LED encoder is common anode.
    testEncoder = new InfScrollEncoder(
        PIN_IN1, PIN_IN2, PIN_BTN, 50,
        PIN_LED_R_SX1509, PIN_LED_G_SX1509, PIN_LED_B_SX1509, INPUT_PULLUP
    );

    testEncoder->attachLedPinModeCallback(
        [](uint8_t pin, uint8_t mode)
        {
            (void)mode;
            io.pinMode(pin, ANALOG_OUTPUT);
        }
    );

    testEncoder->attachLedWriteCallback(
        [](uint8_t pin, uint8_t value)
        {
            uint8_t trueValue = 255 - value;
            io.analogWrite(pin, trueValue);
        }
    );

    testEncoder->begin();

    manager = new InputManager();
    manager->registerInputSource(
        InputSource{testEncoder->getInfScrollEventQueueHandle(), InputSource::Type::InfScrollEncoder}
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
