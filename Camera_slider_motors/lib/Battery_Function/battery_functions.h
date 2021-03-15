#ifndef __BATTERY_FUNCTION_H__
#define __BATTERY_FUNCTION_H__

#include <Arduino.h>

#define BATTERY A0
#define HIGH_BATTERY_VOLTAGE 14.7
#define LOW_BATTERY_VOLTAGE (70*HIGH_BATTERY_VOLTAGE)/100.0

class Battery{
private:
    float _battery_level;

public:
    Battery();
    ~Battery(){};

    float getBatteryLevel(void) {updateBatteryLevel(); return _battery_level;};
    byte getBatteryPercentage(void){
        updateBatteryLevel();
        return (isBatteryLow())?0:((((_battery_level-LOW_BATTERY_VOLTAGE))/(HIGH_BATTERY_VOLTAGE-LOW_BATTERY_VOLTAGE))*100.0);}
    bool isBatteryLow(void){return (_battery_level <= LOW_BATTERY_VOLTAGE);}

private:
    void updateBatteryLevel(void);

};

#endif