#ifndef __CORE_FUNCTION_H__
#define __CORE_FUNCTION_H__

#include "motor_functions.h"
#include "radio_functions.h"
#include "battery_functions.h"
#include "EEPROM_functions.h"
#include "battery_functions.h"



#define TARGET_ARRAY_SIZE 15
#define PLATFORM_HEIGHT 220


class InstructionHandler{
public:
    MotorInterface* _motor;
    Camera* _camera;
    Battery* _battery;
    TELECOMMAND _instructions;
    POSE _start_pose;
    POSE _end_pose;
    POSE _target_pose[TARGET_ARRAY_SIZE];
    short _object_pose[3] = {10000, -1, -1};
    byte _number_of_pose = 0;
    unsigned int _number_of_photo = 0;
    byte _current_pose_index = 0;
    bool _play = false;

    unsigned long _start_time = 0;
    unsigned long _current_time = 0;
    bool _start = false;


public:
    InstructionHandler(MotorInterface* Mot, Camera* Cam, Battery* Batt) :
    _motor(Mot), _camera(Cam), _battery(Batt){}
    ~InstructionHandler(){}

    void execute(const TELECOMMAND& instruction){
        _instructions = instruction;
        parseTelecommandInstructions();
    };

private:
    void parseTelecommandInstructions(void);
    void parseMenu(void);
    void parseState(void);

    bool findHomeMode(void);
    
    void registerNumericMode(void);
    bool programmedMode(void);
    bool programmedMode(const bool& automode);
    void autoMode(void);

    void manualMode(void);

    void registerTrackingMode(void);
    bool trackingMode(void);
    void interpolationTracking(const short object_pose[3], const unsigned short& start_slider, const unsigned short& end_slider);
    float computePanAngleFromPose(const short& object_pose_opposed_length, const short& object_pose_adjacent_length, const unsigned short& camera_pose_slider);
    float computeTiltAngleFromPose(const short& object_pose_opposed_length, const short& object_pose_adjacent_length);
    void clearTrackingPose(void){_object_pose[0] = 10000; _object_pose[1] = -1;_object_pose[2] = -1;}
    
    // bool timelapseMode(void);
    void interpolationHyperlapse(const POSE& start_pose, const POSE& end_pose, int& number_of_photo_left);
    void registerHyperlapseMode(void);
    bool hyperlapseMode(void);

    bool panoramiclapseMode(void);
    
    void addPose(const POSE& newpose);
    void addPose(const int newpose[3]);
    void clearLastPose(void);
    void clearAllPose(void);
    void resetAll(void);
    bool targetPoseFull(void){return (_number_of_pose == TARGET_ARRAY_SIZE);}
    
    void motorTeleop(void);
    bool motorExec(void);


private:
    void play(void);
    void pause(void);
    void stop(void);
};



#endif