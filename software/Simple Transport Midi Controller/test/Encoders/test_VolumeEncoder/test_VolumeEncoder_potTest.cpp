#include <Arduino.h>
#include <Wire.h>
#include <unity.h>
#include <VolumeEncoder.h>
#include <SparkFunSX1509.h>


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

// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte SX1509_ADDRESS = 0x3E; // SX1509 I2C address
SX1509 io;                        // Create an SX1509 object to be used throughout

uint8_t SX1509_testEncoder_sw_pin = 9;

uint8_t testEncoder_W1_pin = 12;
uint8_t testEncoder_W2_pin = 27;

uint8_t ARDUINO_INT_PIN = 14;

VolumeEncoder *testEncoder = new VolumeEncoder(
    testEncoder_W1_pin,
    testEncoder_W2_pin,
    SX1509_testEncoder_sw_pin,
    25,
    5
);



volatile bool sw_ISR_State = false;
void isr() {
    sw_ISR_State = !sw_ISR_State;
}
void setup() {
    bool I2C1_INIT = Wire1.setPins(SDA,SCL);
    Serial.begin(115200);

    pinMode(SDA, OUTPUT);
    Serial.println("Asserting I2C pins high \n");
    digitalWrite(SDA, HIGH);
    Wire1.setClock(400000);
    Wire1.begin();
    Serial.println("Communication with device succeeded");

    // based Sparkfun API, need to point the SX1509 class to the chosen i2c bus
    if (io.begin(SX1509_ADDRESS, Wire1) == false)
    {
        Serial.println("Failed to communicate. Check wiring and address of SX1509.");
        while (1)
            ; // If we fail to communicate, loop forever.
    };

    testEncoder->begin();


    io.pinMode(SX1509_testEncoder_sw_pin, INPUT_PULLUP);

  io.enableInterrupt(SX1509_testEncoder_sw_pin, FALLING);

  pinMode(ARDUINO_INT_PIN, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(ARDUINO_INT_PIN),
                  isr, FALLING);
    //attachInterrupt(39, isr, CHANGE);
    
    pinMode(LED_BUILTIN,OUTPUT);
};

void loop(){
    digitalWrite(LED_BUILTIN, HIGH);
    testEncoder->sw_read(sw_ISR_State);

    if (testEncoder->sw_wasPressed()) {
        Serial.println("The Volume Encoder button has been pressed");
    }

    Serial.printf("Midi Encoder count is %d\n", testEncoder->getEncoderValue());
    delay(100);
}