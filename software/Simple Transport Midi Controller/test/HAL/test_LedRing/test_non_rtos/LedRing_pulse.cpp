#include <Arduino.h>
#include <unity.h>
#include <LedRing_DUPPA.h>
#include <Wire.h>
#include <i2c_addresses.h>
#include <rSerial.h>

// Non-RTOS LedRing dial test.
// Cycles through a set of colours (one colour per 500 ms period) and, within
// each period, sweeps the dial all the way up and back down. Drives the ring
// directly from loop() with no queue/task involved.

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

static uint8_t colourIdx = 0;

void setup()
{
    rSerial.begin(115200);

    Wire.begin();
    Wire.setClock(400000);

    test_LR->setup();
    test_LR->setLedStartValue(0);

    pinMode(LED_BUILTIN, OUTPUT);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
}

void loop()
{
    const int maxPos = test_LR->getMaxLEDPosition();
    const RGB c = COLOUR_CYCLE[colourIdx];

    // Split the 500 ms window across a full up + down sweep.
    const uint16_t stepDelay = PERIOD_MS / (2 * maxPos);

    rSerial.printf("Colour R:%d G:%d B:%d - Up\n", c.r, c.g, c.b);
    for (int i = 0; i <= maxPos; i++)
    {
        test_LR->dial_setLed(i, c.r, c.g, c.b);
        delay(stepDelay);
    }

    rSerial.println("Down");
    for (int i = maxPos; i >= 0; i--)
    {
        test_LR->clearLed(i);
        delay(stepDelay);
    }

    colourIdx = (colourIdx + 1) % COLOUR_COUNT;
}
