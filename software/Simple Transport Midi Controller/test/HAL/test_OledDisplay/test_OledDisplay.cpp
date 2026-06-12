#include <Arduino.h>
#include <unity.h>
#include <OledDisplay.h>
#include "DisplayManagerImpl.h"


#define OLED_DC    T13
#define OLED_CS    MISO // Using MISO pin as GPIO

// Out-of-line DisplayManager template members live in DisplayManagerImpl.h.
// Explicitly instantiate the concrete manager here so its symbols are emitted
// in this translation unit and the references made by OledDisplay.cpp resolve
// at link time.
template class DisplayManager<u8g2Driver>;

OledDisplay oled;
Manager displayManager;

void setUp(void)
{}

void tearDown(void)
{}

// Sanity check that the HAL object was constructed.
void test_oled_display_instantiated(void)
{
    TEST_ASSERT_NOT_NULL(&oled);
}

void setup()
{
    Serial.begin(115200);

    oled.setupSPI_4Wire(U8X8_PIN_NONE, OLED_DC);
    oled.begin(&displayManager); // spawns the FreeRTOS render task

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_oled_display_instantiated);

}

void loop()
{
    // The OledDisplay render task drives the screen; nothing to do here.
    vTaskDelay(pdMS_TO_TICKS(1000));
}
