#include <Arduino.h>
#include <unity.h>
#include <LedRing_DUPPA.h>
#include <Wire.h>
#include <i2c_addresses.h>

// FreeRTOS LedRing dial test.
// Same visual idea as the non-RTOS test (one colour per 500 ms period, dial
// swept up and back down within each period) but here nothing touches the I2C
// bus directly. A dummy producer task stands in for a real producer (e.g. the
// rotary encoder backend or the TransportManager) and only ever posts desired
// dial states via LedRing::overwriteLedRingDialState(). The LedRing's own
// consumer task (created in begin_dial()) owns the bus and paints the ring.

void setUp(void) {}
void tearDown(void) {}

void test_led_builtin_pin_number(void)
{
    TEST_ASSERT_EQUAL(13, LED_BUILTIN);
}

LedRing *test_LR = new LedRing(I2C_ADDRESS_LED_RING);

struct RGB { uint8_t r, g, b; };

static const RGB COLOUR_CYCLE[] = {
    LED_RING_LED_OFF,
    LED_RING_LED_RED,
    LED_RING_LED_GREEN,
    LED_RING_LED_BLUE
};
static const uint8_t COLOUR_COUNT = sizeof(COLOUR_CYCLE) / sizeof(COLOUR_CYCLE[0]);

// One full up + down dial sweep must fit inside this window.
static const uint16_t PERIOD_MS = 500;

// Dummy producer: mimics an encoder / TransportManager feeding the mailbox.
// It only ever posts absolute dial states - it never calls the ring's drawing
// methods, so the I2C bus stays owned exclusively by the consumer task.
void vDummyDialProducerTask(void *pvParameters)
{
    LedRing *ring = static_cast<LedRing *>(pvParameters);

    const int maxPos = ring->getMaxLEDPosition();
    const uint16_t stepDelay = PERIOD_MS / (2 * maxPos);
    uint8_t colourIdx = 0;

    while (true)
    {
        const RGB c = COLOUR_CYCLE[colourIdx];

        for (int i = 0; i <= maxPos; i++)
        {
            LED_RING_DIAL_STATE state{};
            state.LED = {c.r, c.g, c.b};
            state.POSITION = (uint8_t)i;
            ring->overwriteLedRingDialState(state);
            vTaskDelay(pdMS_TO_TICKS(stepDelay));
        }
        
        for (int i = maxPos; i >= 0; i--)
        {
            LED_RING_DIAL_STATE state{};
            state.LED = {c.r, c.g, c.b};
            state.POSITION = (uint8_t)i;
            ring->overwriteLedRingDialState(state);
            vTaskDelay(pdMS_TO_TICKS(stepDelay));
        }

        colourIdx = (colourIdx + 1) % COLOUR_COUNT;
    }
}

void setup()
{
    Serial.begin(115200);

    Wire.begin();
    Wire.setClock(400000);

    test_LR->setLedStartValue(0);

    // begin_dial() runs setup() and spins up the consumer task + mailbox queue.
    test_LR->begin_dial();

    // Spin up the dummy producer that feeds the mailbox.
    xTaskCreatePinnedToCore(
        vDummyDialProducerTask,
        "dummyDial",
        4096,
        test_LR,
        LED_RING_DIAL_FREERTOS_PRIORITY,
        nullptr,
        1);

    pinMode(LED_BUILTIN, OUTPUT);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
