#ifndef __RADIO_FUNCTION_H__
#define __RADIO_FUNCTION_H__

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#define CE_PIN 9
#define CSN_PIN 10
#define SLIDER_LOW_LIMIT 20
#define SLIDER_UP_LIMIT 980

#define OPTION_SEND 0
#define FIND_HOME_MODE 1
#define MANUAL_MODE 2
#define REGISTER_MANUAL_MODE 3
#define REGISTER_NUMERIC_MODE 4
#define REGISTER_TRACKING_MODE 8
#define REGISTER_HYPERLAPSE_MODE 11
#define PROGRAMMED_MODE 6
#define TRACKING 9
#define TIMELAPSE 10
#define HYPERLAPSE_MODE 12
#define PANORAMICLAPSE 13
#define RESET 14
#define WAITING_FOR_USER_CHOICE 15

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};


typedef struct menu{
    byte menu = 20; // man_comm, programmed_mann, programmed_num, record_traj ,timelapse, tracking, homing
    byte pose_number = -1;
    int PointA[3] = {0, 0, 0};
    byte nb_of_point = 0;
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
    byte option_change = 0;
    byte precision=0;
    byte h_camera = 0;
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
        M1.nb_of_point == M2.nb_of_point
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

class RadioTelecommand{
private:
    RF24* _radio_telecommand;
    rf24_pa_dbm_e _pa_level;
    bool _new_data = false;
    byte _data2send = 0; // 0 : TELECOMMAND,  1 : OPTION 
    bool _pause=false;
    unsigned int _ack_received = 0;

    TELECOMMAND _instructions;
    SLIDER_INFO _slider_info;
    OPTION _slider_option;
    LIMITS _limits;

    unsigned long _last_data_time = 0;
    unsigned long _current_time = 0;

public:
    RadioTelecommand(RF24* radio, const rf24_pa_dbm_e& pa_level = RF24_PA_MIN) : 
    _radio_telecommand(radio), _pa_level(pa_level) {}
    ~RadioTelecommand(){}
    void initRadio(void);

private:
    void setPALevel(const rf24_pa_dbm_e& pa_level){
        if (_pa_level != pa_level){
            _pa_level = pa_level;
            _radio_telecommand->setPALevel(_pa_level);
        }
    }
    bool checkConnection(void);
    bool noConnection(void);

    
public:
    bool sendOption(void);
    bool sendTelecommand(void);


private:
    bool writeOption(void); 
    void receiveSliderInfo(void);
    void receiveLimits(void);

public:
    TELECOMMAND getInstructions(void) const {return _instructions;}
    SLIDER_INFO getSliderInfo(void) const {return _slider_info;}
    unsigned short getSliderPose(void) const {return _slider_info.slider_pose;}
    int getTiltPose(void) const {return _slider_info.tilt_pose;}
    int getPanPose(void) const {return _slider_info.pan_pose;}
    
    unsigned short getSliderSpeed(void) const {return _slider_info.slider_speed;}
    unsigned short getTiltSpeed(void) const {return _slider_info.tilt_speed;}
    unsigned short getPanSpeed(void) const {return _slider_info.pan_speed;}

    byte getBattery(void) const {return _slider_info.battery_level;}
    LIMITS getLimits(void) const {return _limits;}
    int getLowLimitSlider() const {return _limits.slider_limits[0];}
    int getUpLimitSlider() const {return _limits.slider_limits[1];}
    int getLowLimitTilt() const {return (_limits.tilt_limits[0]==180)?0:_limits.tilt_limits[0];}
    int getUpLimitTilt() const {return (_limits.tilt_limits[1]==180)?360:_limits.tilt_limits[1];}
    int getLowLimitPan() const {return (_limits.pan_limits[0]==180)?0:_limits.pan_limits[0];}
    int getUpLimitPan() const {return (_limits.pan_limits[1]==180)?360:_limits.pan_limits[1];}

    void setInstruction(const TELECOMMAND& instruction){_instructions = instruction;}
    void setOption(const OPTION& option){_slider_option = option;}

    void setJoystick(const unsigned short& joyx, const unsigned short& joyy){_instructions.JoyX = joyx; _instructions.JoyY = joyy;}
    void setJoystick(const unsigned short joy[2]){setJoystick(joy[0], joy[1]);}
    void setPotentiometer(const byte& potx, const byte& potpt){_instructions.PotentiometerX = potx; _instructions.PotentiometerPT = potpt;}
    void setPotentiometer(byte pot[2]){setPotentiometer(pot[0], pot[1]);}

    void setState(const char& state){_instructions.state = state;}
    void setMenu(const byte& menu){_instructions.menu.menu = menu;}
    void setPoseNumber(const byte& pose_number){_instructions.menu.pose_number = pose_number;}
    void setNumberOfPoint(const byte& nb){_instructions.menu.nb_of_point = nb;}
    void setNextSliderPoint(const int& point){_instructions.menu.PointA[0] = point;}
    void setNextTiltPoint(const int& point){ _instructions.menu.PointA[1] = point;}
    void setNextPanPoint(const int& point){_instructions.menu.PointA[2] = point;}
    void setNextPoint(const int nextpoint[3]){
        setNextSliderPoint(nextpoint[0]);
        setNextTiltPoint(nextpoint[1]);
        setNextPanPoint(nextpoint[2]);
    }
    void setFocus(const bool& focus){_instructions.focus = focus;}
    void setShutter(const bool& shutter){_instructions.shutter = shutter;}
};


#endif
