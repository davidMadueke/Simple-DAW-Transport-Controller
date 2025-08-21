#include <Arduino.h>
#include <unity.h>
#include <SysPowerSwitch.h>

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

volatile bool ISR_State = false;
void isr() {
    ISR_State = !ISR_State;
}
int HW_POWER_SWITCH_PIN = 32;
SysPowerSwitch *testPowerSwitch = new SysPowerSwitch(HW_POWER_SWITCH_PIN); 

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN,OUTPUT);

    pinMode(testPowerSwitch->getPin(), INPUT_PULLUP);
    attachInterrupt(testPowerSwitch->getPin(), isr, CHANGE);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
    UNITY_END(); // stop unit testing
}

void loop()
{   
    testPowerSwitch->read(ISR_State);
    if (testPowerSwitch->isSwitchedOn())
    {
        Serial.write("Switch State is on ");
    }
    

    digitalWrite(LED_BUILTIN, HIGH);
    
}