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
#include <TransportManager.h>
#include <BluetoothModule.h>
#include <BLUETOOTH_MIDI_STATE.h>
#include <deviceNames.h>

#define PIN_IN1 A2
#define PIN_IN2 A3
#define PIN_BTN A4
#define PIN_LED_R_SX1509 0
#define PIN_LED_G_SX1509 1
#define PIN_LED_B_SX1509 2

#ifndef TEST_TRANSPORT_MANAGER_CONSUMER_STACK_SIZE
    #define TEST_TRANSPORT_MANAGER_CONSUMER_STACK_SIZE 4096
#endif

#ifndef TEST_TRANSPORT_MANAGER_CONSUMER_PRIORITY
    #define TEST_TRANSPORT_MANAGER_CONSUMER_PRIORITY 2
#endif

byte SX1509_ADDRESS = 0x3E;
SX1509 io;

InfScrollEncoder* testEncoder = nullptr;
static InputManager* inputMgr = nullptr;
static TransportManager* transportMgr = nullptr;
static BLUETOOTH_MIDI_STATE bleState{false, false, false};
static BluetoothModule* bleModule = nullptr;

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
        case DISPLAY_ACTION::TYPE::INF_SCROLL_DELTA: return "INF_SCROLL_DELTA";
        case DISPLAY_ACTION::TYPE::VOL_ENCODER_MODE_INCREMENTED: return "VOL_ENCODER_MODE_INCREMENTED";
        case DISPLAY_ACTION::TYPE::BLE_CONNECTED: return "BLE_CONNECTED";
        case DISPLAY_ACTION::TYPE::BUTTON_PRESS: return "BUTTON_PRESS";
        default: return "UNKNOWN";
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
            if (action.type == DISPLAY_ACTION::TYPE::INF_SCROLL_MODE_INCREMENTED)
            {
                rSerial.printf(
                    "[DISPLAY] type=%s infScrollMode=%u\n",
                    displayActionTypeToString(action.type),
                    static_cast<unsigned>(action.inf_scroll_mode)
                );
            }
            else if (action.type == DISPLAY_ACTION::TYPE::INF_SCROLL_DELTA)
            {
                rSerial.printf(
                    "[DISPLAY] type=%s midiCC=%u midiValue=%u ch=%u %s\n",
                    displayActionTypeToString(action.type),
                    action.midi_packet.data1,
                    action.midi_packet.data2,
                    action.midi_packet.channel,
                    action.midi_packet.isCoarse ? "Coarse" : "Fine"
                );
            }
            else
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

void test_outbound_queue_is_shared_with_input_manager(void)
{
    TEST_ASSERT_NOT_NULL(inputMgr);
    TEST_ASSERT_NOT_NULL(transportMgr);
    TEST_ASSERT_NOT_NULL(inputMgr->getMidiQueue());
    TEST_ASSERT_NOT_NULL(transportMgr->getMidiSendQueue());
    TEST_ASSERT_EQUAL_PTR(inputMgr->getMidiQueue(), transportMgr->getMidiSendQueue());
}

void setup()
{
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

    pinMode(LED_BUILTIN, OUTPUT);

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

    inputMgr = new InputManager();
    inputMgr->registerInputSource(
        InputSource{testEncoder->getInfScrollEventQueueHandle(), InputSource::Type::InfScrollEncoder}
    );
    inputMgr->begin();

    bleModule = new BluetoothModule(LED_BUILTIN, &bleState);
    bleModule->setDigitalLedWriteCallback(
        [](uint8_t pin, uint8_t value) { digitalWrite(pin, value); }
    );

    transportMgr = new TransportManager(inputMgr, bleModule);
    transportMgr->begin(MIDI_CHANNEL);

    xTaskCreate(
        vDisplayConsumerTask,
        "disp_cons",
        TEST_TRANSPORT_MANAGER_CONSUMER_STACK_SIZE,
        inputMgr,
        TEST_TRANSPORT_MANAGER_CONSUMER_PRIORITY,
        nullptr
    );

    UNITY_BEGIN();
    //RUN_TEST(test_outbound_queue_is_shared_with_input_manager);


}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(100));
}
