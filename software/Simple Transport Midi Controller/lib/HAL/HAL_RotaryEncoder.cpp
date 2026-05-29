#include "Arduino.h"
#include "HAL_RotaryEncoder.h"

void HAL_RotaryEncoder::begin(){
    _encoder = new RotaryEncoder(_pinEnc1, _pinEnc2, RotaryEncoder::LatchMode::TWO03);

    // Create the FreeRTOS processing task, passing 'this' instance as a parameter
    xTaskCreatePinnedToCore(vEncoderTask, "HAL_EncTask", 2048, this, 3, (TaskHandle_t*)&hdl_encoderTask, 1);

    // Link the hardware interrupts directly to the Static Wrappers
    attachInterrupt(digitalPinToInterrupt(_pinEnc1), std::bind(&HAL_RotaryEncoder::ISR_Encoder, this), CHANGE);
    attachInterrupt(digitalPinToInterrupt(_pinEnc2), std::bind(&HAL_RotaryEncoder::ISR_Encoder, this), CHANGE);

    _button = new PushButton(_pinBtn, _dbTime);
    _button->begin(PushButtonDelivery::Polling);
}

 void HAL_RotaryEncoder::vEncoderTask(void *pvParameters){
  HAL_RotaryEncoder* instance = static_cast<HAL_RotaryEncoder*>(pvParameters);
  instance->processTaskLoop();
}

void HAL_RotaryEncoder::processTaskLoop(){
  int lastPos = 0;

  while (true) {
    // Wait indefinitely until any notification bit is set. 
    // Clears bits on exit so we don't re-process old events.
    if (ulTaskNotifyTake( pdTRUE, portMAX_DELAY )) {
        
      int newPos = _encoder->getPosition();
      if (newPos != lastPos) {
          //Serial.printf("Position: %d | Direction: %d\n", newPos, (int)_encoder->getDirection());
          //lastPos = newPos;

          // accelerate when there was a previous rotation in the same direction.

      unsigned long ms = _encoder->getMillisBetweenRotations();

      if (ms < longCutoff) {
        // do some acceleration using factors a and b

        // limit to maximum acceleration
        if (ms < shortCutoff) {
          ms = shortCutoff;
        }

        float ticksActual_float = a * ms + b;
        Serial.print("  f= ");
        Serial.println(ticksActual_float);

        long deltaTicks = (long)ticksActual_float * (newPos - lastPos);
        Serial.print("  d= ");
        Serial.println(deltaTicks);

        newPos = newPos + deltaTicks;
        _encoder->setPosition(newPos);
      }

      Serial.print(newPos);
      Serial.print("  ms: ");
      Serial.println(ms);
      lastPos = newPos;
    }
      }
      }
    }

void IRAM_ATTR HAL_RotaryEncoder::ISR_Encoder(){
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  _encoder->tick();

  vTaskNotifyGiveFromISR(hdl_encoderTask, &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}