#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(){

}

void BatteryMonitor::setup(){
    //We can call begin() as it does not initialise the Wire class
    while (!maxlipo.begin()) {
    Serial.println(F("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!"));
    m_noBatteryConnected = true;
    delay(2000);
  }
  float init_chargeRate = this->getCellChargeRate();
float init_cellVoltage = this->getCellVoltage();
float init_cellPercentage = this->getCellPercentage();
bool init_isCharging = this->isCharging();
bool init_lowPower = this->isLowPower();

}

bool BatteryMonitor::isBatteryPresent(){
    if (maxlipo->isDeviceReady()){
        m_noBatteryConnected = false;
        return true;}
    else {
        m_noBatteryConnected = true:
        return false;
    }
    
}

float BatteryMonitor::getCellVoltage(){
    if (m_noBatteryConnected){
        return -1
    }
    float m_cellVoltage = maxlipo->cellVoltage();
    return m_cellVoltage;
}

float BatteryMonitor::getCellPercentage(){
    if (m_noBatteryConnected){
        return -1
    }
    float m_cellPercentage = maxlipo->cellPercent();
    return m_cellPercentage;
}

float BatteryMonitor::getCellChargeRate(){
    if (m_noBatteryConnected){
        return -1
    }
    float m_cellChargeRate = maxlipo->chargeRate();
    return m_cellChargeRate;
}

bool BatteryMonitor::isCharging(){
    if (m_cellChargeRate >= 0){
        m_isCharging = true;
        return false
    }
    else {
        m_isCharging = false;
        return false;
    }
}

bool BatteryMonitor::isLowPower(){
    if (maxlipo->isActiveAlert() & 
        (maxlipo->getAlertStatus() & MAX1704X_ALERTFLAG_SOC_LOW)){
        
        m_isLowPower = true;
    }

    return m_isLowPower;
}