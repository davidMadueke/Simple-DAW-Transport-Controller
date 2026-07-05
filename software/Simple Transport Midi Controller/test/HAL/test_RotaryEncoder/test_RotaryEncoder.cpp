#include <Arduino.h>
#include <unity.h>
#include <Wire.h>
#include <HAL_RotaryEncoder.h>
#include <rSerial.h>

#define PIN_IN1 A2
#define PIN_IN2 A3
#define PIN_BTN A4  // Encoder push-button pin

void setUp(void)
{}

void tearDown(void)
{}

void test_led_builtin_pin_number(void)
{
  TEST_ASSERT_EQUAL(13, LED_BUILTIN); 
}

HAL_RotaryEncoder* test_module = nullptr;

void setup()
{
    rSerial.begin(115200);

    test_module = new HAL_RotaryEncoder(PIN_IN1, PIN_IN2, PIN_BTN, 50, INPUT_PULLDOWN);

    test_module->begin();

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);

};


void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}