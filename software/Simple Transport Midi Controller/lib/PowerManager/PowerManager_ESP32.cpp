#include "PowerManager_ESP32.h"
#include "constants.h"
#include "driver/rtc_io.h"

PowerManager_ESP32::PowerManager_ESP32(uint8_t pin_pwr_switch)
{
    pwr_switch = new SysPowerSwitch(pin_pwr_switch);
    // RTC_DATA_ATTR int bootCount = 0; // If we wish to add important small data to RTC memory that is preserved even during deep sleep
}
void PowerManager_ESP32::setup_pwrSwitch_as_extWakeUp()
{
    pinMode(this->getPin(), INPUT_PULLUP);
    esp_sleep_enable_ext1_wakeup_io(BUTTON_PIN_BITMASK(PWR_WAKEUP_GPIO), ESP_EXT1_WAKEUP_ANY_HIGH);
    rtc_gpio_hold_en(PWR_WAKEUP_GPIO);
    rtc_gpio_pulldown_en(PWR_WAKEUP_GPIO);  // This GPIO pin is tied to GND in order to wake up in HIGH
    rtc_gpio_pullup_dis(PWR_WAKEUP_GPIO);   // Disable PULL_UP in order to allow it to wakeup on HIGH
}

bool PowerManager_ESP32::readOnOffButton(bool State)
{
    return pwr_switch->read(State);
}

void PowerManager_ESP32::sysTurnOff()
{
    this->setup_pwrSwitch_as_extWakeUp();
    esp_deep_sleep_start();
}

uint8_t PowerManager_ESP32::getPin()
{
    return pwr_switch->getPin();
}
