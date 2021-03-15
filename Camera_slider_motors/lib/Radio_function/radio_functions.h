#ifndef __RADIO_FUNCTIONS_H__
#define __RADIO_FUNCTIONS_H__

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "camera_functions.h"
#include "battery_functions.h"
#include "motor_functions.h"

#define OPTION_SEND 0
#define FIND_HOME_MODE 1
#define MANUAL_MODE 2
#define REGISTER_MANUAL_MODE 3
#define REGISTER_NUMERIC_MODE 4
#define REGISTER_TRAJECTORY 5
#define PROGRAMMED_MODE 6
#define AUTOMODE 7
#define REGISTER_TRACKING_MODE 8
#define TRACKING 9
#define TIMELAPSE 10
#define REGISTER_HYPERLPSE 11
#define HYPERLAPSE 12
#define PANORAMICLAPSE 13
#define RESET 14
#define WAITING_FOR_USER_CHOICE 15

#define BUZZER A7

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};


typedef struct menu{
    byte menu = 20; // man_comm, programmed_mann, programmed_num, record_traj ,timelapse, tracking, homing
    byte pose_number = -1;
    int PointA[3] = {0, 0, 0};
    uint16_t ms_delay = 0;
} MENU;

typedef struct telecommand {
    char state = 'p';          // Play Pause
    MENU menu;           // man_comm, programmed_mann, programmed_num, record_traj ,timelapse, tracking, homing
    unsigned short JoyX = 0;
    unsigned short JoyY = 0;     // Joysick value
    byte PotentiometerX = 0;
    byte PotentiometerPT = 0;// Potentiometer value
    bool shutter = 0;        // Shutter Camera
    bool focus = 0;          // Focus Camera
} TELECOMMAND;


typedef struct option {
    byte precision = 1;
    byte h_camera = 0;
    byte l_camera = 0;
    byte l_lens = 0;
    byte r_lens = 0;
    unsigned short slider_limits[2] = {short(SLIDER_LOW_LIMIT), short(SLIDER_UP_LIMIT)};
    unsigned short pan_limits[2] = {180, 180};
    bool active = false;
    bool auto_speed = true;
    bool auto_mode = false;
    unsigned int delay = 0;
} OPTION;

typedef struct limits{
    bool ack = true;
    int slider_limits[2] = {int(SLIDER_LOW_LIMIT), int(SLIDER_UP_LIMIT)};
    int tilt_limits[2] = {180, 180};
    int pan_limits[2] = {180, 180}; 
} LIMITS;

typedef struct slider_info{
    bool ack = true;
    byte pt_nb = 0;
    unsigned short slider_pose = 0;
    int tilt_pose = 0;
    int pan_pose = 0;
    unsigned short slider_speed = 0;
    unsigned short tilt_speed = 0;
    unsigned short pan_speed = 0;
    byte battery_level = 100;
} SLIDER_INFO;


inline bool operator==(const MENU &M1, const MENU& M2){
    return(
        M1.menu == M2.menu &&
        M1.pose_number == M2.pose_number &&
        M1.PointA[0] == M2.PointA[0] && M1.PointA[1] == M2.PointA[1] && M1.PointA[2] == M2.PointA[2] &&
        M1.ms_delay == M2.ms_delay
    );
}

inline bool operator==(const TELECOMMAND& T1, const TELECOMMAND& T2){
    return (
        T1.menu == T2.menu &&
        T1.state == T2.state &&
        T1.JoyX == T2.JoyX && T1.JoyY == T2.JoyY &&
        T1.PotentiometerPT == T2.PotentiometerPT && T1.PotentiometerX == T2.PotentiometerX &&
        T1.shutter == T2.shutter && T1.focus == T2.focus
    );
}

class RadioMotor{
private:
    RF24* _radio_motor;
    Camera* _camera;
    rf24_pa_dbm_e _pa_level;
    bool _connection;
    bool _new_data = false;
    byte _data2receive = 0; // 0 : TELECOMMAND,  1 : OPTION 

    TELECOMMAND _instructions;
    TELECOMMAND _last_instructions;
    SLIDER_INFO _slider_info;
    OPTION _slider_option;
    LIMITS _limits;

    unsigned long _last_data_time = 0;
    unsigned long _current_time = 0;
    unsigned int _data_received_counter = 0;

public:
    RadioMotor(RF24* radio, Camera* camera, const rf24_pa_dbm_e& pa_level = RF24_PA_MIN) : 
    _radio_motor(radio), _camera(camera), _pa_level(pa_level) {}
    ~RadioMotor(){}
    void initRadio(void);

private:    
    void setPALevel(const rf24_pa_dbm_e& pa_level){
        if (_pa_level != pa_level){
            _pa_level = pa_level;
            _radio_motor->setPALevel(_pa_level);
        }
    }
    bool checkConnection(void);
    bool noConnection(void);
    void alertNoConnection(void);

private:
    void updateMotorInfo(const MotorInterface& Mot);
    void updateBatteryInfo(Battery& bat);
    void updateSliderInfo(const MotorInterface& Mot, Battery& batt, const bool ack){
        _slider_info.ack = ack;
        updateMotorInfo(Mot);
        //updateBatteryInfo(batt);
        _radio_motor->writeAckPayload(1, &_slider_info, sizeof(_slider_info));
    }

    
private:
    void receiveOption(MotorInterface& Mot);
    void setOptions(MotorInterface& Mot);
    bool receiveTelecommand( const MotorInterface& Mot, Battery& batt);

    void updateLimits(const bool &ack);

public:
    bool receive(MotorInterface& Mot, Battery& batt);

    bool isTelecommandInstructionNew(void)
    {
        if (!(_last_instructions == _instructions))
        {
            _last_instructions = _instructions;
            return true;
        }
        return true;
    }

    void getJoystick(unsigned short& joyx, unsigned short& joyy){joyx = _instructions.JoyX; joyy = _instructions.JoyY;}
    void getJoystick(unsigned short joy[2]){getJoystick(joy[0], joy[1]);}
    void getPotentiometer(byte& potx, byte& potpt){potx = _instructions.PotentiometerX; potpt = _instructions.PotentiometerPT;}
    void getPotentiometer(byte pot[2]){getPotentiometer(pot[0], pot[1]);}

    char getState(void){return _instructions.state;}
    byte getMenu(void){return _instructions.menu.menu;}
    int getNextSliderPoint(void){return _instructions.menu.PointA[0];}
    int getNextTiltPoint(void){return _instructions.menu.PointA[1];}
    int getNextPanPoint(void){return _instructions.menu.PointA[2];}
    void getNextPoint(int nextpoint[3]){
        nextpoint[0] = getNextSliderPoint();
        nextpoint[1] = getNextTiltPoint();
        nextpoint[2] = getNextPanPoint();
    }
    bool getFocus(void){return _instructions.focus;}
    bool getShutter(void){return _instructions.shutter;}

    TELECOMMAND getInstructions(void) const {return _instructions;}
    OPTION getOptions(void) const {return _slider_option;}
};
#endif