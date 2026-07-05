#include "Arduino.h"
#include "HAL_RotaryEncoder.h"
#include <rSerial.h>

void HAL_RotaryEncoder::begin()
{
    _encoder = new RotaryEncoder(_pinEnc1, _pinEnc2, RotaryEncoder::LatchMode::TWO03); 
    _button = new PushButton("ROT_ENC",_pinBtn, _dbTime, false, _buttonInputMode);
    _button->begin(PushButtonDelivery::Queue, nullptr, ROTARY_ENCODER_BUTTON_FREERTOS_EVENT_QUEUE_LENGTH);
    m_buttonQueue = _button->getButtonEventQueueHandle();

    
    // create the Binary Semaphore for the encoder ISR
    smphr_isr_encoder = xSemaphoreCreateBinary();

    // Create a queue to feed both of the buttons and encoder events
    m_rotaryEncoderQueue = xQueueCreate(ROTARY_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH, sizeof(RotaryEncoderEvent));

    // Create a queue set with Button Queue Length + 1 (for the binary smphr) set size
    m_queueSet = xQueueCreateSet(1 + ROTARY_ENCODER_BUTTON_FREERTOS_EVENT_QUEUE_LENGTH);
    xQueueAddToSet(smphr_isr_encoder, m_queueSet);
    xQueueAddToSet(m_buttonQueue, m_queueSet);

    // Create the FreeRTOS processing task, passing 'this' instance as a parameter
    xTaskCreatePinnedToCore(vEncoderTask, "HAL_EncTask", ROTARY_ENCODER_FREERTOS_TASK_STACK_SIZE, this, ROTARY_ENCODER_FREERTOS_PRIORITY, (TaskHandle_t*)&hdl_encoderTask, 1);

    // Link the hardware interrupts directly to the Static Wrappers
    attachInterrupt(digitalPinToInterrupt(_pinEnc1), std::bind(&HAL_RotaryEncoder::ISR_Encoder, this), CHANGE);
    attachInterrupt(digitalPinToInterrupt(_pinEnc2), std::bind(&HAL_RotaryEncoder::ISR_Encoder, this), CHANGE);
}

 void HAL_RotaryEncoder::vEncoderTask(void *pvParameters)
 {
  HAL_RotaryEncoder* instance = static_cast<HAL_RotaryEncoder*>(pvParameters);
  instance->processTaskLoop();
}

void HAL_RotaryEncoder::processTaskLoop()
{
  int lastPos = 0;

  while (true) 
  {
    QueueSetMemberHandle_t xActivatedMember = xQueueSelectFromSet( m_queueSet,
             200 / portTICK_PERIOD_MS );

    if ( xActivatedMember == smphr_isr_encoder)
    {
      xSemaphoreTake(smphr_isr_encoder, 0); // Block time 0 as Queue set member has already revealed a semaphore is ready to be taken
      
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
        rSerial.print("  f= ");
        rSerial.println(ticksActual_float);

        long deltaTicks = (long)ticksActual_float * (newPos - lastPos);
        rSerial.print("  d= ");
        rSerial.println(deltaTicks);

        newPos = newPos + deltaTicks;
        _encoder->setPosition(newPos);
      }

      rSerial.print(newPos);
      rSerial.print("  ms: ");
      rSerial.println(ms);

      RotaryEncoderEvent ev{};
      ev.type     = RotaryEncoderEvent::Type::Encoder;
      ev.encValue = (uint8_t)newPos;
      ev.delta = (uint8_t)(newPos - lastPos); // Guarunteed to be != 0 due to the queue set selection
      xQueueSend(m_rotaryEncoderQueue, &ev, 0);
      lastPos = newPos;
    }
      }
    
    else if (xActivatedMember == m_buttonQueue)
    {
      PushButtonEvent btnEv{};
      // 0 timeout: select already guaranteed an item is present.
      if (xQueueReceive(m_buttonQueue, &btnEv, 0) == pdTRUE) 
      {
          RotaryEncoderEvent ev{};
          ev.type     = RotaryEncoderEvent::Type::Button;
          ev.btnEvent = btnEv;
          xQueueSend(m_rotaryEncoderQueue, &ev, 0);
      }
    }
  }

    
}


void IRAM_ATTR HAL_RotaryEncoder::ISR_Encoder()
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  _encoder->tick();

  xSemaphoreGiveFromISR(smphr_isr_encoder, &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}