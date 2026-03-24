#include <Arduino.h>
#include <Adafruit_Max1704X.h>

class BatteryMonitor {
    private:
    Adafruit_MAX17048 *maxlipo;
    uint8_t m_batteryCheckRate;
    bool m_noBatteryConnected;

    bool m_isLowPower = false;
    bool m_isCharging ;
    float m_cellVoltage;
    float m_cellPercentage;
    float m_cellChargeRate;


    public:

    void setup();

    //Returns true or false depending on whether the Device submits a valid I2C status to indicate that a battery is presently connected to it
    // If false then the m_noBatteryConnected flag is asserted and the I2C data functions do not return anything
    bool isBatteryPresent();

    float getCellVoltage();

    float getCellPercentage();

    bool isLowPower();

    float getCellChargeRate();

    // Uses the discharge rate to indicate if the battery is charging, a positive charge rate would indicate that the battery is being charged
    bool isCharging();

};