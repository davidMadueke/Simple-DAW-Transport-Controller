

#include "Arduino.h"
#include <RotaryEncoder.h> // Matthias Hertel Rotary Encoder Lib
#include <PushButton.h>

class HAL_RotaryEncoder
{
  public:
    HAL_RotaryEncoder(uint8_t pinEnc1, uint8_t pinEnc2, uint8_t pinBtn, uint8_t dbTime) : 
      _pinEnc1(pinEnc1), _pinEnc2(pinEnc2), _pinBtn(pinBtn), _dbTime(dbTime) {};
    
    void begin();

    static void vEncoderTask(void *pvParameters);

    void ISR_Encoder();

    void processTaskLoop();

  private:
  // Task handles for encoder tasks
  volatile TaskHandle_t hdl_encoderTask;

  // Rotary Encoder instance
  RotaryEncoder* _encoder = nullptr;

  // Push Button instance
  PushButton* _button = nullptr;

  // Hardware Pins
  uint8_t _pinEnc1, _pinEnc2, _pinBtn, _dbTime;

  // Define some constants (For encoder Acceleration functionality).

  // the maximum acceleration is 10 times.
  float m = 10;

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