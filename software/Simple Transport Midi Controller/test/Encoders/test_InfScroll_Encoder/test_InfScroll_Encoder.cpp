#include <Arduino.h>
#include <unity.h>
#include <Wire.h>
#include <infScrollEncoder.h>
#include <DISPLAY_STATE.h>
#include <SparkFunSX1509.h>

#define PIN_IN1 A2
#define PIN_IN2 A3
#define PIN_BTN A4  // Encoder push-button pin
#define PIN_LED_R_SX1509 0
#define PIN_LED_G_SX1509 1
#define PIN_LED_B_SX1509 2                                                                                                                                                                                                       

void setUp(void)
{}

void tearDown(void)
{}

void test_led_builtin_pin_number(void)
{
  TEST_ASSERT_EQUAL(13, LED_BUILTIN); 
}

InfScrollEncoder* test_module = nullptr;

byte SX1509_ADDRESS = 0x3E; // SX1509 I2C address
SX1509 io; 

//DISPLAY_STATE g_displayState{};
//portMUX_TYPE g_displayStateMux = portMUX_INITIALIZER_UNLOCKED;

void setup()
{
    Serial.begin(115200);
     Wire1.setPins(SDA,SCL);
    pinMode(SDA, OUTPUT);
    Serial.println("Asserting I2C pins high \n");
    digitalWrite(SDA, HIGH);

    Wire1.begin();
    Serial.println("Communication with device succeeded");
    if (io.begin(SX1509_ADDRESS, Wire1) == false)
    {
    Serial.println("Failed to communicate. Check wiring and address of SX1509.");
    while (1)
      ; // If we fail to communicate, loop forever.
    }
    // io.pinMode(0, OUTPUT);
    // io.digitalWrite(0, HIGH);

    Serial.println("Startup successful");
    

    //  NOTE: The Duppa RGB Led Encoder we are using is common anode
    test_module = new InfScrollEncoder(PIN_IN1, PIN_IN2, PIN_BTN, 50,
        PIN_LED_R_SX1509, PIN_LED_G_SX1509, PIN_LED_B_SX1509, INPUT_PULLUP);
    
    //test_module->attachDisplayState(&g_displayState, &g_displayStateMux);

    test_module->attachLedPinModeCallback
    (
      [](uint8_t pin, uint8_t mode) 
      { 
        io.pinMode(pin, ANALOG_OUTPUT); // required for pwm
        // We are using the SX1509's PWM signal, thus it is necessary to initiaise led
        //io.ledDriverInit(pin);
      }
    );

    test_module->attachLedWriteCallback
    (
      [](uint8_t pin, uint8_t value) 
      { 
        //Using PWM to set the LED intensity
        uint8_t trueValue = 255 - value; // Because RGB led is common cathode a PWM of 100% turns it off
        io.analogWrite(pin, trueValue);
      }
    );

    test_module->begin();
    
    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);

};


void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
    // io.digitalWrite(0, HIGH);
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // io.digitalWrite(0, LOW);
    // vTaskDelay(pdMS_TO_TICKS(1000));
}