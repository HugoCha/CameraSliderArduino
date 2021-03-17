#include "EEPROM_functions.h"

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
    const unsigned short& up_tilt_lim)
{
    saveAutoSpeed(autospeed);
    saveMicrostepping(microstepping);
    saveAutomode(automode);
    saveDelay(delay);
    saveSliderLimits(low_slider_lim, up_slider_lim);
    savePanLimits(low_pan_lim, up_pan_lim);
    saveTiltLimits(low_tilt_lim, up_tilt_lim);
}


void saveAutoSpeed(const bool& autospeed)
{
    bool autospeed_test = false;
    EEPROM.get(EEPROM_AUTOSPEED_ADDRESS, autospeed_test);
    if (autospeed_test != autospeed)
        EEPROM.put(EEPROM_AUTOSPEED_ADDRESS, autospeed);
}

void saveMicrostepping(const byte& microstepping)
{
    byte microstepping_test = 0;
    EEPROM.get(EEPROM_MICROSTEPPING_ADDRESS, microstepping_test);
    if (microstepping_test != microstepping)
        EEPROM.put(EEPROM_MICROSTEPPING_ADDRESS, microstepping);
}


void saveAutomode(const bool& automode)
{
    bool automode_test = false;
    EEPROM.get(EEPROM_AUTOMODE_ADDRESS, automode_test);
    if (automode_test != automode)
        EEPROM.put(EEPROM_AUTOMODE_ADDRESS, automode);
}


void saveDelay(const unsigned short& delay)
{
    unsigned short delay_test = 0;
    EEPROM.get(EEPROM_DELAY_ADDRESS, delay_test);
    if (delay_test != delay)
        EEPROM.put(EEPROM_DELAY_ADDRESS, delay);
}

void saveSliderLimits(const unsigned short& low_slider_lim, const unsigned short& up_slider_lim)
{
    unsigned short low_lim_test = 0;
    unsigned short up_lim_test = 0;
    EEPROM.get(EEPROM_SLIDER_LOW_LIMIT_ADDRESS, low_lim_test);
    EEPROM.get(EEPROM_SLIDER_UP_LIMIT_ADDRESS, up_lim_test);
    if (low_lim_test != low_slider_lim)
        EEPROM.put(EEPROM_SLIDER_LOW_LIMIT_ADDRESS, low_slider_lim);
    if (up_lim_test != up_slider_lim)
        EEPROM.put(EEPROM_SLIDER_UP_LIMIT_ADDRESS, up_slider_lim);
}

void savePanLimits(const unsigned short& low_pan_lim, const unsigned short& up_pan_lim)
{
    unsigned short low_lim_test = 0;
    unsigned short up_lim_test = 0;
    EEPROM.get(EEPROM_PAN_LOW_LIMIT_ADDRESS, low_lim_test);
    EEPROM.get(EEPROM_PAN_UP_LIMIT_ADDRESS, up_lim_test);
    if (low_lim_test != low_pan_lim)
        EEPROM.put(EEPROM_PAN_LOW_LIMIT_ADDRESS, low_pan_lim);
    if (up_lim_test != up_pan_lim)
        EEPROM.put(EEPROM_PAN_UP_LIMIT_ADDRESS, up_pan_lim);
}



void saveTiltLimits(const unsigned short& low_tilt_lim, const unsigned short& up_tilt_lim)
{
    unsigned short low_lim_test = 0;
    unsigned short up_lim_test = 0;
    EEPROM.get(EEPROM_TILT_LOW_LIMIT_ADDRESS, low_lim_test);
    EEPROM.get(EEPROM_TILT_UP_LIMIT_ADDRESS, up_lim_test);
    if (low_lim_test != low_tilt_lim)
        EEPROM.put(EEPROM_TILT_LOW_LIMIT_ADDRESS, low_tilt_lim);
    if (up_lim_test != up_tilt_lim)
        EEPROM.put(EEPROM_TILT_UP_LIMIT_ADDRESS, up_tilt_lim);
}


void setEEPROM(MotorInterface& Mot)
{
    bool autospeed = false;
    byte microstepping = 0;
    bool automode = false;
    unsigned short delay = 0;
    unsigned short low_slider_lim = 0;
    unsigned short up_slider_lim = 0;
    unsigned short low_pan_lim = 0; 
    unsigned short up_pan_lim = 0;
    unsigned short low_tilt_lim=0;
    unsigned short up_tilt_lim=0;

    EEPROM.get(EEPROM_AUTOSPEED_ADDRESS, autospeed);
    EEPROM.get(EEPROM_MICROSTEPPING_ADDRESS, microstepping);
    EEPROM.get(EEPROM_AUTOMODE_ADDRESS, automode);
    EEPROM.get(EEPROM_DELAY_ADDRESS, delay);
    EEPROM.get(EEPROM_SLIDER_LOW_LIMIT_ADDRESS, low_slider_lim);
    EEPROM.get(EEPROM_SLIDER_UP_LIMIT_ADDRESS, up_slider_lim);
    EEPROM.get(EEPROM_PAN_LOW_LIMIT_ADDRESS, low_pan_lim);
    EEPROM.get(EEPROM_PAN_UP_LIMIT_ADDRESS, up_pan_lim);
    EEPROM.get(EEPROM_TILT_LOW_LIMIT_ADDRESS, low_tilt_lim);
    EEPROM.get(EEPROM_TILT_UP_LIMIT_ADDRESS, up_tilt_lim);
    
    Mot.setAutoSpeed(autospeed);
    Mot.setAutoMode(automode);
    Mot.setMicrostepping(microstepping);
    Mot.setDelay((unsigned long)delay*1000);
    Mot.setLowLimit({(float)low_slider_lim, (float)low_tilt_lim, (float)low_pan_lim});
    Mot.setUpLimit({(float)up_slider_lim, (float)up_tilt_lim, (float)up_pan_lim});
}