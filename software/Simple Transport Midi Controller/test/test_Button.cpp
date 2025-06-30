#include <Arduino.h>
#include <unity.h>
#include <Button.h>

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

unsigned long msLast;           // last time the LED was switched
volatile unsigned long lastPressTime = 0;

const unsigned long
    LONG_PRESS(1000),           // we define a "long press" to be 1000 milliseconds.
    BLINK_INTERVAL(100);        // in the BLINK state, switch the LED every 100 milliseconds.


Button *testButton = new Button(0x25, 25); //Pin A3

volatile bool ISR_State = false;
void isr() {
    ISR_State = !ISR_State;
}
void setup() {
    bool I2C1_INIT = Wire1.setPins(SDA,SCL);
    Serial.begin(115200);

    pinMode(SDA, OUTPUT);
    Serial.println("Asserting I2C pins high \n");
    digitalWrite(SDA, HIGH);
    Wire1.begin();
    Serial.println("Communication with device succeeded");
    
    testButton->ledOff();
    testButton->setSinglePressLedIndicator(128, 128, 128);
    testButton->setBrightness(10);
    testButton->setMultiPressTimer(150);
    pinMode(LED_BUILTIN,OUTPUT);

    pinMode(39, INPUT_PULLUP);
    attachInterrupt(39, isr, CHANGE);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
    UNITY_END(); // stop unit testing
}


// the list of possible states for the state machine. This state machine has a fixed
// sequence of states, i.e. ONOFF --> TO_BLINK --> BLINK --> TO_ONOFF --> ONOFF
// note that while the user perceives two "modes", i.e. ON/OFF mode and rapid blink mode,
// two extra states are needed in the state machine to transition between these modes.
enum states_t {ONOFF, TO_BLINK, BLINK, TO_ONOFF};

bool ledState;                  // current LED status
unsigned long ms;               // current time from millis()

void longPressTest()
{
    static states_t STATE;      // current state machine state
    ms = millis();              // record the current time
    testButton->read(ISR_State);            // read the button

    switch (STATE)
    { 
        // this state watches for short and long presses, switches the LED for
        // short presses, and moves to the TO_BLINK state for long presses.
        case ONOFF:
            if (testButton->wasReleased())
            {
                msLast = ms;                // record the last switch time
                ledState = !ledState;
                if(ledState) testButton->setLedColour(255,255,255);
                if(!ledState) testButton->ledOff();
                
            }
            else if (testButton->pressedFor(LONG_PRESS))
                STATE = TO_BLINK;
            break;

        // this is a transition state where we start the fast blink as feedback to the user,
        // but we also need to wait for the user to release the button, i.e. end the
        // long press, before moving to the BLINK state.
        case TO_BLINK:
            testButton->setLedColour(255,255,0);
            if (testButton->wasReleased())
                STATE = BLINK;
            //else
            //    testButton->setLedColour(255,255,0);
            break;

        // the fast-blink state. Watch for another long press which will cause us to
        // turn the LED off (as feedback to the user) and move to the TO_ONOFF state.
        case BLINK:
            if (testButton->pressedFor(LONG_PRESS))
            {
                STATE = TO_ONOFF;
                ledState = false;
            }
            else
            {
                if(!testButton->isPressed()){
                    testButton->setLedColour(255,255,0);
                    Serial.println("yello");
                }
                
            } 
                
            break;

        // this is a transition state where we just wait for the user to release the button
        // before moving back to the ONOFF state.
        case TO_ONOFF:
            if (testButton->wasReleased())
                STATE = ONOFF;
            break;
    }
}

void loop()
{
    //longPressTest();
    //testButton->read(ISR_State);
    uint8_t numOfPresses = testButton->multiPressRead(ISR_State);
    switch (numOfPresses){
        case 1:
          Serial.println("single press!");
          if(millis() - msLast > 100)
          {
            testButton->setLedColour(255,0,0);
            msLast = millis();
            break;
            
            }
            break;
          
        case 2:
          Serial.println("double press!");
          testButton->setLedColour(0,255,0);

          break;
        case 3:
          Serial.println("triple press!");
          testButton->setLedColour(0,0,255);
          break;
        default:
          // do nothing
          // for no press and presses over 3 in fast succession
          break;
      }
    digitalWrite(LED_BUILTIN, HIGH);
}