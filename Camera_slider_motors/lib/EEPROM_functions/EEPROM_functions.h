#ifndef __EEPROM_CONFIG_H__
#define __EEPROM_CONFIG_H__

#include <Arduino.h>
#include <EEPROM.h>
#include "motor_functions.h"

#define EEPROM_AUTOSPEED_ADDRESS 0
#define EEPROM_MICROSTEPPING_ADDRESS 1
#define EEPROM_AUTOMODE_ADDRESS 2
#define EEPROM_DELAY_ADDRESS 3
#define EEPROM_SLIDER_LOW_LIMIT_ADDRESS 5
#define EEPROM_SLIDER_UP_LIMIT_ADDRESS 7
#define EEPROM_TILT_LOW_LIMIT_ADDRESS 9
#define EEPROM_TILT_UP_LIMIT_ADDRESS 11
#define EEPROM_PAN_LOW_LIMIT_ADDRESS 13
#define EEPROM_PAN_UP_LIMIT_ADDRESS 15

void saveEEPROM(
    const bool& autospeed,
    const byte& microstepping,
    const bool& automode,
    const unsigned short& delay,
    const unsigned short& low_slider_lim, 
    const unsigned short& up_slider_lim,
    const unsigned short& low_pan_lim, 
    const unsigned short& up_pan_lim,
    const unsigned short& low_tilt_lim, 
    const unsigned short& up_tilt_lim
    );
void saveAutoSpeed(const bool& autospeed);
void saveMicrostepping(const byte& microstepping);
void saveAutomode(const bool& automode);
void saveDelay(const unsigned short& delay);
void saveSliderLimits(const unsigned short& low_slider_lim, const unsigned short& up_slider_lim);
void savePanLimits(const unsigned short& low_pan_lim, const unsigned short& up_pan_lim);
void saveTiltLimits(const unsigned short& low_tilt_lim, const unsigned short& up_tilt_lim);
void setEEPROM(MotorInterface& Mot);

#endif