#include <Arduino.h>
#include <unity.h>
#include <Button.h>
#include <Wire.h>
#include <i2c_addresses.h>
#include <pins.h>

#ifndef TEST_BTN_I2C_ADDR
#define TEST_BTN_I2C_ADDR I2C_BTN_METRONOME_INT
#endif

#ifndef TEST_BTN_INT_PIN
#define TEST_BTN_INT_PIN 6
#endif

Button* testButton = new Button("TEST", TEST_BTN_I2C_ADDR, TEST_BTN_INT_PIN, 25);;

unsigned long msLast = 0;

const unsigned long LONG_PRESS_MS = 500;

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

    while (!Serial) {
    delay(10); // Wait for the serial port to connect
    }

    testButton->begin(PushButtonDelivery::Polling);
    
    testButton->ledOff();
    testButton->setSinglePressLedIndicator(128, 0, 128);
    testButton->setBrightness(15);
    testButton->setMultiPressTimer(150);
    testButton->setLongPressTime(LONG_PRESS_MS);
    

    UNITY_BEGIN();
    RUN_TEST(test_button_constructed);
}

enum states_t { ONOFF, TO_BLINK, BLINK, TO_ONOFF };

bool ledState = false;

void longPressTest()
{
    static states_t STATE = ONOFF;

    switch (STATE) {
        case ONOFF:
            if (testButton->consumeLongPressEdge()) {
                //testButton->pressedFor(LONG_PRESS_MS)
                STATE = TO_BLINK;
            }
            break;

        case TO_BLINK:
            testButton->setLedColour(255, 255, 0);
            Serial.println(" To Long Press State");
            if (testButton->consumeReleaseEdge()) {
                STATE = BLINK;
            }
            break;

        case BLINK:
            if (testButton->consumeLongPressEdge()) {
                STATE = TO_ONOFF;
            } 
            break;

        case TO_ONOFF:
            if (testButton->consumeReleaseEdge()) {
                Serial.println(" To Single Press State");
                testButton->ledOff();
                STATE = ONOFF;
            }
            break;
    }
}

void multiPressTest(){
    uint8_t numOfPresses = testButton->getAndClearMultiPress();
    switch (numOfPresses) {
        case 1:
            Serial.println("single press!");
            if (millis() - msLast > 100) {
                testButton->setLedColour(255, 0, 0);
                msLast = millis();
            }
            break;
        case 2:
            Serial.println("double press!");
            testButton->setLedColour(0, 255, 0);
            break;
        case 3:
            Serial.println("triple press!");
            testButton->setLedColour(0, 0, 255);
            break;
        default:
            break;
    }

    if (testButton->consumePressEdge()) {
        Serial.println("Press edge");
    }

    if (testButton->consumeLongPressEdge()) {
        Serial.println("Long press");
    }
}

void loop()
{
    longPressTest();
    //multiPressTest();
    

    // static uint32_t last = 0;
    // if (millis() - last > 1000) {
    //     Serial.println("loop alive");
    //     last = millis();
    // }

    //vTaskDelay(pdMS_TO_TICKS(10));
}
