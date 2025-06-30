#include <Arduino.h>
#include <DFRobot_RGBButton.h>

void setup() {
  bool I2C1_INIT = Wire1.setPins(SDA,SCL);
  Serial.begin(115200);

  pinMode(SDA, OUTPUT);
  Serial.println("Asserting I2C pins high \n");
  digitalWrite(SDA, HIGH);
  Wire1.begin();
  Serial.println("Communication with device succeeded");
  
}

void loop() {
}