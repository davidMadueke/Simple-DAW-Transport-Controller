#include <Arduino.h>
#include <unity.h>
#include <PowerManager_ESP32.h>
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

volatile bool ISR_State = true;
void isr() {
    ISR_State = !ISR_State;
}
int HW_POWER_SWITCH_PIN = 32;

PowerManager_ESP32 *testPowerManager = new PowerManager_ESP32(HW_POWER_SWITCH_PIN);
RTC_DATA_ATTR int bootCount = 0;
uint8_t num = 0;

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN,OUTPUT);
    delay(500);

    pinMode(testPowerManager->getPin(), INPUT_PULLUP);
    attachInterrupt(testPowerManager->getPin(), isr, CHANGE);

    //Increment boot number and print it every reboot
    ++bootCount;
    Serial.println("Boot number: " + String(bootCount));

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
    UNITY_END(); // stop unit testing
}

bool dsr = false;
void loop()
{   bool state = testPowerManager->readOnOffButton(ISR_State);
    if(ISR_State != dsr){Serial.println("State ChAnged");}
    if (state)
    {
      //bool a = digitalRead(testPowerManager->getPin());
      //Serial.write(a);
    }
    else
    {
        Serial.write("System Turned off ");
        delay(2000);
        Serial.println("Going to sleep...");
        delay(1000);
        Serial.println("3");
        delay(1000);
        Serial.println("2");
        delay(1000);
        Serial.println("1");
        delay(1000);
        testPowerManager->sysTurnOff();
    }
    
    //Serial.println(String(num));
    dsr = ISR_State;
    //delay(500);
    //num++;
    digitalWrite(LED_BUILTIN, HIGH);
    
}