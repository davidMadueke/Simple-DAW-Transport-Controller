#include <Arduino.h>
#include <unity.h>
#include <MetButton.h>
#include <BUTTON_MIDI_STATE.h>
#include <PushButton.h>
#include <Wire.h>
#include <i2c_addresses.h>
#include <pins.h>
#include <constants.h>

#define TEST_BTN_I2C_ADDR I2C_BTN_METRONOME_INT
#define TEST_BTN_INT_PIN 6
#define TEST_DUMMY_TAPTEMPO_BTN_PIN A4
#define TEST_DUMMY_TAPTEMPO_LED_PIN A5



BUTTON_LED_STATE longPressState = {128, 0, 0};
BUTTON_LED_STATE singlePressState = {128, 128, 128};
BUTTON_LED_STATE toggleLedState = {0, 0, 128};


Jack_TapTempo* dummy_TapTempo = new Jack_TapTempo(TEST_DUMMY_TAPTEMPO_BTN_PIN, BUTTON_DEBOUNCE_TIME_MS);

uint16_t longPressTimeMS = 500u;
MetButton* testButton = new MetButton(
    TEST_BTN_I2C_ADDR, TEST_BTN_INT_PIN, 
    BUTTON_DEBOUNCE_TIME_MS,
    &longPressState,
    &singlePressState,
    &toggleLedState,
    dummy_TapTempo,
    longPressTimeMS);


void setUp(void) {}
void tearDown(void) {}

void test_button_constructed(void)
{
    TEST_ASSERT_NOT_NULL(testButton);
}

void setup()
{
    //Wire1.setPins(SDA, SCL);
    //pinMode(SDA, OUTPUT);
    //digitalWrite(SDA, HIGH);
    //Wire1.begin();
    Serial.begin(115200);
    pinMode(TEST_DUMMY_TAPTEMPO_LED_PIN, OUTPUT);
   
    while (!Serial) {
    delay(10); // Wait for the serial port to connect
    }
    Serial.println("setup Starting");
    testButton->begin(10, 150U, (uint16_t)500U);
    dummy_TapTempo->begin();
    dummy_TapTempo->attachLedWriteCallback([](bool onOff) { digitalWrite(TEST_DUMMY_TAPTEMPO_LED_PIN, onOff);});

    UNITY_BEGIN();
    RUN_TEST(test_button_constructed);
}

PushButtonEvent evt;

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(10));
}
