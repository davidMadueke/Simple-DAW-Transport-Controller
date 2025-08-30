#include <Arduino.h>
#include <unity.h>
#include <LedRing_DUPPA.h>
#include <Wire.h>

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


LedRing *test_LR = new LedRing(ISSI3746_SJ2 | ISSI3746_SJ5);
int BUTTON_BUILTIN = BUTTON; // From ESP32-Feather-V2 Variants
int BUTTON_BUILTIN_ISR = 0; // To change the LEDs colour

uint8_t redVal = 0;
uint8_t greenVal = 0;
uint8_t blueVal = 0;

void isr(){
  BUTTON_BUILTIN_ISR++;
}

uint8_t ledPosition = 0;


void setup() {
    bool I2C1_INIT = Wire1.setPins(SDA,SCL);
    Serial.begin(115200);

    pinMode(SDA, OUTPUT);
    Serial.println("Asserting I2C pins high \n");
    digitalWrite(SDA, HIGH);
    Wire1.begin();
    Wire.begin();
    Wire.setClock(400000);
    Serial.println("Communication with device succeeded");
    //delay(3000);
    //Serial.println("Testing first test_lr command");
    //delay(3000);

    test_LR->setup();
    test_LR->setLedStartValue(0);

    pinMode(LED_BUILTIN,OUTPUT);
    pinMode(BUTTON_BUILTIN,INPUT);
    attachInterrupt(BUTTON_BUILTIN, isr, RISING);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
    UNITY_END(); // stop unit testing
}

void loop(){
     
    if(BUTTON_BUILTIN_ISR > 3){BUTTON_BUILTIN_ISR = 0;}

    switch(BUTTON_BUILTIN_ISR){
      case 0:
        redVal = 255;
        greenVal = 0;
        blueVal = 0;
        break;
      
      case 1:
        redVal = 0;
        greenVal = 255;
        blueVal = 0;
        break;
      
      case 2:
        redVal = 0;
        greenVal = 0;
        blueVal = 255;
        break;
    }
    // Turn the dial all the way
    Serial.println("Up");
    for(int i = 0; i < test_LR->getMaxLEDPosition(); i++)
    {
        test_LR->setLedColourState(i,redVal, greenVal, blueVal);
        LedRing::Led_Colour_State colourState = test_LR->getLedColourState(i);
        Serial.printf("LED %d - R: %d, G: %d, B: %d\n", i, colourState.red, colourState.green, colourState.blue);
        test_LR->dial_setLed(i, redVal, greenVal, blueVal);
        delay(50);
    }
  
    delay(2000);
    
    Serial.println("Down");
    // Turn the dial all the way back
    for(int i = test_LR->getMaxLEDPosition(); i > 0; i--)
    {
        test_LR->setLed(i, 0, 0, 0);
        delay(50);
    }

    delay(2000);

    uint8_t prevMillis = millis();
    digitalWrite(LED_BUILTIN, HIGH);
}