#include <Arduino.h>
#include <Wire.h>
#include <unity.h>
#include "constants.h"
#include "i2c_addresses.h"
#include <VolumeEncoder.h>


void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
}

void test_led_builtin_pin_number(void)
{
  TEST_ASSERT_EQUAL(13, LED_BUILTIN);
}


#define PIN_IN1 A2
#define PIN_IN2 A3
#define PIN_BTN A4  // Encoder push-button pin

#define TEST_LED_RING_START_VALUE 5U

VolumeEncoder *testModule = new VolumeEncoder(
    PIN_IN1, 
    PIN_IN2, 
    PIN_BTN, 
    50,
    I2C_ADDRESS_LED_RING,
    INPUT_PULLUP
);

void setup()
{
    rSerial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);


    Serial.println("Startup successful");
    
    testModule->begin(TEST_LED_RING_START_VALUE);
    
    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);

};

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}