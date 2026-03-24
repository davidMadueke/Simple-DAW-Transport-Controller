#include <Arduino.h>
#include <unity.h>
#include <Wire.h>
/* #include <SparkFunSX1509.h> */
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
#include "deviceNames.h"
#include <BluetoothModule.h>
#include <BLUETOOTH_MIDI_STATE.h>
#include "test_simple_MIDI_IF_Manager.h"

void setUp(void)
{}

void tearDown(void)
{}

void test_led_builtin_pin_number(void)
{
  TEST_ASSERT_EQUAL(13, LED_BUILTIN);
}

int BUTTON_BUILTIN = BUTTON; // From ESP32-Feather-V2 Variants
int LED_SX1509_PIN = 0;

// Initialise BLUETOOTH MIDI STATE
BLUETOOTH_MIDI_STATE* MIDI_STATE = new BLUETOOTH_MIDI_STATE{false, false, false, false};

BLEMIDI_CREATE_INSTANCE(BT_DEVICE_NAME, MIDI_bt) 

BluetoothModule* test_module = new BluetoothModule(
    BUTTON_BUILTIN,/*Enable Pin*/
    LED_BUILTIN,
    MIDI_STATE
);

MIDI_IF* test_midi_interface = new MIDI_IF(test_module, MIDI_STATE);

/*
// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte SX1509_ADDRESS = 0x3E; // SX1509 I2C address
SX1509* io = new SX1509();                        // Create an SX1509 object to be used throughout
*/
// Create Bluetooth Module Enable Button ISR
volatile bool BUTTON_BUILTIN_ISR_State = false;
void BUTTON_BUILTIN_isr() {
    BUTTON_BUILTIN_ISR_State = !BUTTON_BUILTIN_ISR_State;
}

void ReadCB(void *parameter);

void setup()
{
    Wire1.setPins(SDA,SCL);
    Serial.begin(115200);

    pinMode(SDA, OUTPUT);
    Serial.println("Asserting I2C pins high \n");
    digitalWrite(SDA, HIGH);
    Wire1.begin();
    Wire.begin();
    Wire.setClock(400000);
    Serial.println("Communication with device succeeded");

    pinMode(LED_BUILTIN,OUTPUT);
    pinMode(BUTTON_BUILTIN,INPUT);
    attachInterrupt(BUTTON_BUILTIN, BUTTON_BUILTIN_isr, CHANGE);

    /*
    //i2c_Bus_Scan(Wire);
    // based Sparkfun API, need to point the SX1509 class to the chosen i2c bus
    if (io->begin(SX1509_ADDRESS) == false)
    {
        Serial.println("Failed to communicate. Check wiring and address of SX1509.");
        while (1)
            ; // If we fail to communicate, loop forever.
    };
    io->pinMode(LED_SX1509_PIN, OUTPUT);
*/
    // Setup the digitalWrite Callback to be the SX1509 API
    // NOTE callbacks need to be non-static, hence we use a lambda to capture the object pointer and call the function
    test_module->setDigitalLedWriteCallback(
        [](uint8_t pin, uint8_t value) { /* io-> */digitalWrite(pin, value); }
    );

    test_module->createInstance(&BLEMIDI_bt, &MIDI_bt);
    //test_midi_interface->initialise();
    /*
    xTaskCreatePinnedToCore(
        ReadCB,           //See FreeRTOS for more multitask info  
        "MIDI-READ",
        3000,
        NULL,
        1,
        NULL,
        1
    );*/

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
    UNITY_END(); // stop unit testing
    //digitalWrite(LED_BUILTIN, HIGH);
    /* io->digitalWrite(LED_SX1509_PIN, HIGH);
    delay(2000);
    io->digitalWrite(LED_SX1509_PIN, HIGH); */
};

uint8_t numOfPresses = 0;
unsigned long t0 = millis();

void loop()
{
    Serial.printf("Starting Loop...");
    //MIDI_bt.read();
    
    //uint8_t numOfPresses = test_module->buttonStateMachine(BUTTON_BUILTIN_ISR_State);

    //auto MIDI = test_midi_interface->getInterface();

    test_midi_interface->stateMachine(&BLEMIDI_bt, &MIDI_bt);

    int printToTerminalTime = 4000 /*ms*/;
    
    if (MIDI_STATE->enable && (millis() - t0) > 1000)
    {
        t0 = millis();

        MIDI_bt.sendNoteOn(60, 100, 1); // note 60, velocity 100 on channel 1
        //vTaskDelay(250/portTICK_PERIOD_MS);
        //MIDI_bt.sendNoteOff(60, 0, 1);
    }
    if (MIDI_STATE->enable && (millis() - t0) > 2000)
    {
        t0 = millis();

        //MIDI_bt.sendNoteOn(60, 100, 1); // note 60, velocity 100 on channel 1
        //vTaskDelay(250/portTICK_PERIOD_MS);
        MIDI_bt.sendNoteOff(60, 0, 1);
    }

    if ((millis() - t0) > printToTerminalTime){
        Serial.printf("Number of presses: %d\n", numOfPresses);
        Serial.printf("Current Bluetooth Midi Enable State: %s\n", 
            (MIDI_STATE->enable ? "true" : "false"));

        t0 = millis();
    }
    
};

void ReadCB(void *parameter)
{
    test_midi_interface->FREERTOS_MIDI_Read_CB(&MIDI_bt);
}

