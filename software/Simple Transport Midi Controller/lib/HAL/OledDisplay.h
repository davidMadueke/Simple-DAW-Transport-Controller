#pragma once
#include <U8g2lib.h>
#include <DisplayManager.h>

#define OLED_DISPLAY_SSD1306_128X64_SPI
#define OLED_DISPLAY_BUFFER_MODE_FULL


#ifndef OLED_DISPLAY_FREERTOS_PRIORITY
    #define OLED_DISPLAY_FREERTOS_PRIORITY 5
#endif
#ifndef OLED_DISPLAY_FREERTOS_TASK_STACK_SIZE
    #define OLED_DISPLAY_FREERTOS_TASK_STACK_SIZE 4096
#endif

#ifdef OLED_DISPLAY_SSD1306_128X64_SPI
    using u8g2Driver = U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI ;
    using Manager = DisplayManager<u8g2Driver>;
#endif

class OledDisplay
{
    private:
    u8g2Driver* _driver = nullptr;
    Manager* _manager = nullptr;
    TaskHandle_t hdl_oledTask = nullptr;

    void renderTaskLoop();

    public:
    void setupSPI_4Wire(uint8_t chipSelectPin, uint8_t dcPin, uint8_t reset = U8X8_PIN_NONE);
    
    // Begin fn to initialise the task 
    void begin(Manager* manager);

    static void vOledDisplayTask(void* pvParameters);

    

};