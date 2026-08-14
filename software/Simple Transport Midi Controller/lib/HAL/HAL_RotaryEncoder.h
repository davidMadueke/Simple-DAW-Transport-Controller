
#include "Arduino.h"
#include <RotaryEncoder.h> // Matthias Hertel Rotary Encoder Lib
#include <PushButton.h>
#include <functional>
#include <freertos/queue.h>
#include <freertos/semphr.h> 
#pragma once

#ifndef ROTARY_ENCODER_FREERTOS_PRIORITY
    #define ROTARY_ENCODER_FREERTOS_PRIORITY 8
#endif

#ifndef ROTARY_ENCODER_FREERTOS_TASK_STACK_SIZE
    #define ROTARY_ENCODER_FREERTOS_TASK_STACK_SIZE 2048
#endif

#ifndef ROTARY_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH
    #define ROTARY_ENCODER_FREERTOS_EVENT_QUEUE_LENGTH 8
#endif

#ifndef ROTARY_ENCODER_BUTTON_FREERTOS_EVENT_QUEUE_LENGTH
    #define ROTARY_ENCODER_BUTTON_FREERTOS_EVENT_QUEUE_LENGTH 8
#endif

#ifndef ROTARY_ENCODER_ENCODER_POSITION_MAX_ACCELERATION
    #define ROTARY_ENCODER_ENCODER_POSITION_MAX_ACCELERATION 10
#endif

struct RotaryEncoderEvent {
    enum class Type : uint8_t {
        Encoder,
        Button
    } type;

    PushButtonEvent btnEvent = {};
    uint8_t encValue = 0;
    int16_t delta = 0;

};

class HAL_RotaryEncoder
{
  public:
    HAL_RotaryEncoder(uint8_t pinEnc1, uint8_t pinEnc2, uint8_t pinBtn, uint8_t dbTime , uint8_t pinBtnInputMode = INPUT_PULLUP) : 
      _pinEnc1(pinEnc1), _pinEnc2(pinEnc2), _pinBtn(pinBtn), _dbTime(dbTime), _buttonInputMode(pinBtnInputMode) {};
    
    void begin();

    static void vEncoderTask(void *pvParameters);

    void ISR_Encoder();

    void processTaskLoop();

    QueueHandle_t getRotaryEncoderEventQueueHandle() {return m_rotaryEncoderQueue;};

    // Sets the Pushbutton Long Press timer to zero (disabling it)
    void disableLongPresses() {_button->setLongPressTimer((uint8_t)0U);};

  private:
  // Task handles for encoder tasks
  volatile TaskHandle_t hdl_encoderTask;

  // Queue Handles for Rotary Encoder + Button
  QueueHandle_t m_rotaryEncoderQueue = nullptr;
  QueueHandle_t m_buttonQueue = nullptr;

  // Combined Queue Set handle to parse both button queue and encoder smphr simultaneously
  QueueSetHandle_t m_queueSet = nullptr;

  // Binary Semaphore Handle for Encoder ISR
  SemaphoreHandle_t smphr_isr_encoder = nullptr;

  // Rotary Encoder instance
  RotaryEncoder* _encoder = nullptr;

  // Push Button instance
  PushButton* _button = nullptr;

  // Hardware Pins
  uint8_t _pinEnc1, _pinEnc2, _pinBtn, _dbTime;

  // Button Input Mode
  uint8_t _buttonInputMode;

  // Define some constants (For encoder Acceleration functionality).

  // the maximum acceleration is 10 times.
  float m = ROTARY_ENCODER_ENCODER_POSITION_MAX_ACCELERATION;

  // at 200ms or slower, there should be no acceleration. (factor 1)
  float longCutoff = 50;

  // at 5 ms, we want to have maximum acceleration (factor m)
  float shortCutoff = 5;

  // To derive the calc. constants, compute as follows:
  // On an x(ms) - y(factor) plane resolve a linear formular factor(ms) = a * ms + b;
  // where  f(4)=10 and f(200)=1

  float a = (m - 1) / (shortCutoff - longCutoff);
  float b = 1 - longCutoff * a;


};