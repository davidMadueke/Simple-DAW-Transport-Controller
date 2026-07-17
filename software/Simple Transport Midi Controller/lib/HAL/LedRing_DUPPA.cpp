#include "LedRing_DUPPA.h"

LedRing::LedRing(uint8_t I2C_LedRing_Addr) {
    Ring = new LEDRingSmall(I2C_LedRing_Addr);
    m_startLedValue = 0;
}

void LedRing::setup() {
    
    Ring->LEDRingSmall_Reset();
  vTaskDelay(pdMS_TO_TICKS(20));

  Ring->LEDRingSmall_Configuration(0x01); //Normal operation
  Ring->LEDRingSmall_PWMFrequencyEnable(1);
  Ring->LEDRingSmall_SpreadSpectrum(0b0010110);
  Ring->LEDRingSmall_GlobalCurrent(0x04);
  Ring->LEDRingSmall_SetScaling(0xF0);
  Ring->LEDRingSmall_PWM_MODE();
}

void LedRing::reset(){
    Ring->LEDRingSmall_Reset();
}

void LedRing::clearAll(){
    Ring->LEDRingSmall_ClearAll();
}

void LedRing::buffer_flush(){
    Ring->LEDRingSmall_Buffer_Flush();
}

void LedRing::clearLed(uint8_t logicalLEDPosition, bool bufferMode){
    this->setLed(logicalLEDPosition, 0, 0, 0, bufferMode);
}

void LedRing::setLedStartValue(uint8_t startValue){
    m_startLedValue = constrain(startValue, 0, getMaxLEDPosition());
}

void LedRing::setLed(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b, bool bufferMode){
    int actualLedPosition = getLEDPosition(logicalLEDPosition);
    if (!bufferMode)
    {
        Ring->LEDRingSmall_Set_RED(actualLedPosition, r);
        Ring->LEDRingSmall_Set_GREEN(actualLedPosition, g);
        Ring->LEDRingSmall_Set_BLUE(actualLedPosition, b);
    }
    else 
    {
        Ring->LEDRingSmall_Buffer_Set_Led(actualLedPosition, r, g, b);
    }
    
}

void LedRing::setLedFromState(uint8_t logicalLEDPosition, bool bufferMode){
    int actualLedPosition = getLEDPosition(logicalLEDPosition);
    LED_RING_LED_STATE lcs = getLedColourState(logicalLEDPosition);
    
    if (!bufferMode)
    {
        Ring->LEDRingSmall_Set_RED(actualLedPosition, lcs.Red);
        Ring->LEDRingSmall_Set_GREEN(actualLedPosition, lcs.Green);
        Ring->LEDRingSmall_Set_BLUE(actualLedPosition, lcs.Blue );
    }
    else 
    {
        Ring->LEDRingSmall_Buffer_Set_Led(actualLedPosition, lcs.Red, lcs.Green, lcs.Blue);
    }
    
}


void LedRing::dial_setLed(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b){
    for (int i = 0; i <= logicalLEDPosition; i++)              setLed(i, r, g, b, true);
    for (int i = logicalLEDPosition + 1; i < TOTAL_LEDS; i++)  setLed(i, 0, 0, 0, true);  // clear tail

    buffer_flush();
}

void LedRing::dial_setLedFromState(uint8_t logicalLEDPosition){
    for (int i = 0; i <= logicalLEDPosition; i++)              setLedFromState(i, true);
    for (int i = logicalLEDPosition + 1; i < TOTAL_LEDS; i++)  setLed(i, 0, 0, 0, true);  // clear tail

    buffer_flush();
}

LED_RING_LED_STATE LedRing::getLedColourState(uint8_t logicalLEDPosition)
{
    int pos = getLEDPosition(logicalLEDPosition);
    return m_ledColourStates[pos];
}

void LedRing::setLedColourState(uint8_t logicalLEDPosition, uint8_t r, uint8_t g, uint8_t b) {
        int pos = getLEDPosition(logicalLEDPosition);
        LED_RING_LED_STATE newState = {r, g, b};
        m_ledColourStates[pos] = newState;
}

/* FREE RTOS ASSOCIATED METHODS */

void LedRing::begin_dial()
{
    setup();
    // queue length set to one - implementing mailbox queue data structure
    m_ledRingDialQueue = xQueueCreate(1, sizeof(LED_RING_DIAL_STATE));


    // Create the FreeRTOS processing task, passing 'this' instance as a parameter
    xTaskCreatePinnedToCore(vLedRingDialTask, "HAL_LEDRingTask", LED_RING_DIAL_FREERTOS_TASK_STACK_SIZE, this, LED_RING_DIAL_FREERTOS_PRIORITY, (TaskHandle_t*)&hdl_ledRingDialTask, 1);

}


void LedRing::vLedRingDialTask(void *pvParameters)
{
    LedRing* instance = static_cast<LedRing*>(pvParameters);
    instance->processTaskLoop();
}

void LedRing::processTaskLoop()
{
    LED_RING_DIAL_STATE dial{};
    while (true)
    {
        if (xQueueReceive(m_ledRingDialQueue, &dial, portMAX_DELAY) == pdTRUE)
        {
            dial_setLed(dial.POSITION, dial.LED.Red, dial.LED.Green, dial.LED.Blue);
        }   
    }
}