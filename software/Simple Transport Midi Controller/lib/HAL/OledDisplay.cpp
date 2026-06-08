#include "OledDisplay.h"


void OledDisplay::setupSPI(uint8_t chipSelectPin, uint8_t dcPin, uint8_t reset){
    //uint8_t rstPin = (reset != NULL) ? reset : (uint8_t)U8X8_PIN_NONE;
    _driver = u8g2Driver(U8G2_R0, /* cs=*/ chipSelectPin, /* dc=*/ dcPin, /* reset=*/ ((reset != NULL) ? reset : U8X8_PIN_NONE));
}

void OledDisplay::begin(Manager* manager){
    _manager = manager;
    _driver.begin();
    xTaskCreatePinnedToCore(
        vOledDisplayTask, 
        "OledTask",
         OLED_DISPLAY_FREERTOS_TASK_STACK_SIZE, 
         this, 
         OLED_DISPLAY_FREERTOS_PRIORITY, 
         &hdl_oledTask,
         1);
}


void OledDisplay::vOledDisplayTask(void* pvParameters) {
    static_cast<OledDisplay*>(pvParameters)->renderTaskLoop();
}
void OledDisplay::renderTaskLoop() {
    while (true) {
        #ifdef OLED_DISPLAY_BUFFER_MODE_2
            _manager->processPendingActions();   // drain queue → reducer
            _driver.firstPage();
            do {
                _manager->render(_driver);         // draw from current state
            } while (_driver.nextPage());
            vTaskDelay(pdMS_TO_TICKS(33));        // ~30 fps
        #elif defined(OLED_DISPLAY_BUFFER_MODE_FULL)
            _manager->processPendingActions();   // drain queue → reducer
            _driver.firstPage();
            do {
                _manager->render(_driver);         // draw from current state
            } while (_driver.nextPage());
            vTaskDelay(pdMS_TO_TICKS(33));        // ~30 fps
        #endif
    }
}