#include "battery_functions.h"

Battery::Battery(){
    pinMode(BATTERY, INPUT);
    updateBatteryLevel();
}

void Battery::updateBatteryLevel(){
    _battery_level = map(analogRead(BATTERY), 0, 1023, 0, int(HIGH_BATTERY_VOLTAGE*100))/100.0;
}
