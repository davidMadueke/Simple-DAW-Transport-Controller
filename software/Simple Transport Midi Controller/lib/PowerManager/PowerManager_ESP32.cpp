#include "PowerManager_ESP32.h"
#include "constants.h"
#include "driver/rtc_io.h"

PowerManager_ESP32::PowerManager_ESP32(uint8_t pin_pwr_switch, uint32_t db_time)
{
    pwr_switch = new SysPowerSwitch(pin_pwr_switch, db_time);
    // RTC_DATA_ATTR int bootCount = 0; // If we wish to add important small data to RTC memory that is preserved even during deep sleep
}

void PowerManager_ESP32::begin(){
    pwr_switch->begin();

    xTaskCreatePinnedToCore(
        vSysPwrTask, "SYS_PWR_TASK", 128, this, 3, &hdl_sysPwrTask, 1);
    
}

void PowerManager_ESP32::vSysPwrTask(void* pvParameters)
{
    static_cast<PowerManager_ESP32*>(pvParameters)->processTaskLoop();
}

void PowerManager_ESP32::processTaskLoop(){
    // If power pin shorted to ground, then turn off system
    // Else leave it on
    while(true){
    if (!pwr_switch->getToggleState()) {
    #ifdef TEST_SYS_PWR_DEBUG
        Serial.write("System Turned off ");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        Serial.println("Going to sleep...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Serial.println("3");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Serial.println("2");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Serial.println("1");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    #endif
        sysTurnOff();
    }}
}

void PowerManager_ESP32::setup_pwrSwitch_as_extWakeUp()
{
    //pinMode(this->getPin(), INPUT_PULLUP); //Encapsulate under PushButton Object
    esp_sleep_enable_ext1_wakeup_io(BUTTON_PIN_BITMASK(PWR_WAKEUP_GPIO), ESP_EXT1_WAKEUP_ANY_HIGH);
    rtc_gpio_hold_en(PWR_WAKEUP_GPIO);
    rtc_gpio_pulldown_en(PWR_WAKEUP_GPIO);  // This GPIO pin is tied to GND in order to wake up in HIGH
    rtc_gpio_pullup_dis(PWR_WAKEUP_GPIO);   // Disable PULL_UP in order to allow it to wakeup on HIGH
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
