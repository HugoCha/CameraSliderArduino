#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <AccelStepper.h>
#include <MultiStepper.h>
//#include <EEPROM.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

// Motor Config

// #define ENABLE 0
// #define MS1 1
// #define MS2 2
// #define MS3 11

#define STEP1 3
#define DIR1 4
#define STEP2 5
#define DIR2 6
#define STEP3 7
#define DIR3 8

#define HALF_STEP 2
#define QUARTER_STEP 4
#define EIGHTH_STEP 8
#define SIXTEENTH_STEP 16


// #define SLIDER_PULLEY_TEETH 20
// #define PAN_GEAR_RATIO 3.0
// #define TILT_GEAR_RATIO 3.0
// #define PULLEY_STEP 2
// #define STEP_PER_TURN 200.0
// #define DEGREE_PER_STEP 360.0/STEP_PER_TURN


// Radio Config
#define CE 10
#define CSN 9

#define BUZZER A7

// Camera Config
// #define SHUTTER 12
// #define FOCUS 13

#define SHUTTER_DELAY 200


// Hall Sensors config
#define HALL_PAN A1
#define HALL_TILT A2
#define IR_SLIDER A3 
// The minimal time (us) needed to consider that a sensor change of state 
#define SENSOR_CHANGE_MIN_TIME 100


// Battery Checker
#define BATTERY A0

// EEPROM Config


// // Slider Limits (mm)
// #define SLIDER_LOW_LIMIT 21.0
// #define SLIDER_UP_LIMIT 980.0



#endif