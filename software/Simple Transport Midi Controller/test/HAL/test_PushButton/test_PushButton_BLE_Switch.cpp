#include <Arduino.h>
#include <unity.h>
#include <PushButton.h>

// BLE switch or any GPIO button with INPUT_PULLUP (LOW = pressed)
#ifndef TEST_BTN_PIN
#define TEST_BTN_PIN 39
#endif

PushButton* testButton = nullptr;

void setUp(void) {}
void tearDown(void) {}

void test_pushbutton_constructed(void)
{
    TEST_ASSERT_NOT_NULL(testButton);
}

void setup()
{
    Serial.begin(115200);
    testButton = new PushButton(TEST_BTN_PIN, 25);
    testButton->setMultiPressTimer(150);
    testButton->setLongPressTime(1000);
    testButton->begin(PushButtonDelivery::Polling);

    UNITY_BEGIN();
    RUN_TEST(test_pushbutton_constructed);
    UNITY_END();
}

void loop()
{
    uint8_t presses = testButton->getAndClearMultiPress();
    if (presses > 0) {
        Serial.printf("Multi-press: %u\n", presses);
    }
    if (testButton->consumePressEdge()) {
        Serial.println("Press edge");
    }
    if (testButton->consumeLongPressEdge()) {
        Serial.println("Long press");
    }
    vTaskDelay(pdMS_TO_TICKS(10));
}
