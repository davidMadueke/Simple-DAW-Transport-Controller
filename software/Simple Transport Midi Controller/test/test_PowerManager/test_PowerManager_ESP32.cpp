#include <Arduino.h>
#include <unity.h>
#include <PowerManager_ESP32.h>
#include <pins.h>
#include "driver/rtc_io.h"


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



PowerManager_ESP32 *testPowerManager = new PowerManager_ESP32(PIN_SYS_SW_PWR, 50u);
RTC_DATA_ATTR int bootCount = 0;
uint8_t num = 0;

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
    UNITY_END(); // stop unit testing
    
    //Increment boot number and print it every reboot
    ++bootCount;
    Serial.println("Boot number: " + String(bootCount));

    testPowerManager->begin();

    
}

void loop()
{   
    digitalWrite(LED_BUILTIN, HIGH);
    
}