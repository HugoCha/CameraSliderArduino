#ifndef __MOTOR_FUNCTIONS_H__
#define __MOTOR_FUNCTIONS_H__

#include <AccelStepper.h>
#include <MultiStepper.h>

// Motor Config

#define IS_IN(a,b,c) ((a-c<=b && b<=a+c)?1:0)

#define ENABLE 0
#define MS1 1
#define MS2 2
#define MS3 11

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

#define MIN_SPEED_STEP_MOTOR 50.0
#define MAX_SPEED_STEP_MOTOR 1200.0


#define SLIDER_PULLEY_TEETH 20
#define PAN_GEAR_RATIO 3.0
#define TILT_GEAR_RATIO 3.0
#define PULLEY_STEP 2
#define STEP_PER_TURN 200.0
#define DEGREE_PER_STEP 360.0/STEP_PER_TURN

// Hall Sensors config
#define HALL_PAN A1
#define HALL_TILT A2
#define IR_SLIDER A3 
// The minimal time (us) needed to consider that a sensor change of state 
#define SENSOR_CHANGE_MIN_TIME 100


// Slider Limits (mm)
#define SLIDER_LOW_LIMIT 20.0
#define SLIDER_UP_LIMIT 980.0

#define JOYSTICK_STOP_ZONE 10

template <typename T, typename T1>
struct Vector3{
    T slider;
    T1 tilt;
    T1 pan;
};

template <typename T, typename T1>
bool operator==(const struct Vector3<T, T1>& V1, const struct Vector3<T, T1>& V2)
{
    return (
        V1.slider==V2.slider && 
        V1.pan == V2.pan && 
        V1.tilt == V2.tilt);
}

template <typename T, typename T1>
bool operator<=(const struct Vector3<T, T1>& V1, const struct Vector3<T, T1>& V2)
{
    return (
        V1.slider<=V2.slider && 
        V1.pan <= V2.pan && 
        V1.tilt <= V2.tilt);
}

typedef struct Vector3<float, float> POSE;
typedef struct Vector3<float,float> SPEED;
typedef struct Vector3<float, float> ACCELERATION;
typedef struct Vector3<long, long> STEPNUMBER;
typedef struct Vector3<bool,bool> SENSOR_STATE;
typedef struct Vector3<bool,bool> DIRECTION;

const POSE no_limit_low = {SLIDER_LOW_LIMIT, 180, 180};
const POSE no_limit_up = {SLIDER_UP_LIMIT, 180, 180};
const SPEED speed_limits = {MAX_SPEED_STEP_MOTOR, MAX_SPEED_STEP_MOTOR, MAX_SPEED_STEP_MOTOR};



class MotorInterface{
// Attributes
private:
    AccelStepper* _slider_stepper;
    AccelStepper* _pan_stepper;
    AccelStepper* _tilt_stepper;
    MultiStepper _steppers;

    byte _micro_stepping;

    POSE _pose_goal;
    POSE _next_pose_goal;
    bool _new_pose_goal=false;
    POSE _current_pose;
    POSE _limits_pose[2];
    byte _pose_number=0;

    SPEED _current_speed;
    SPEED _max_speed;
    ACCELERATION _acceleration;

    STEPNUMBER _step_count;
    DIRECTION _direction;

    SENSOR_STATE _sensor_state;
    bool _slider_obstacle = false;

    bool _auto_speed;
    bool _automode=false;
    bool _manual_mode;
    unsigned long _ms_delay;
    bool _pause=false;

public:
    const POSE _home_pose1 = {0, 0, 0};
    const POSE _home_pose2 = {1000, 0, 0};

// Methods
public:
// Constructors and Destructors
    MotorInterface (AccelStepper* slider_step, AccelStepper* tilt_step, AccelStepper* pan_step, const ACCELERATION& acceleration={500, 500, 500}, const POSE& low_lim = no_limit_low, const POSE& up_lim = no_limit_up, const bool& auto_sp=1, const byte& microstepping = 1);
    MotorInterface(AccelStepper* steppers[3], const ACCELERATION& acceleration={500, 500, 500}, const POSE& low_lim = no_limit_low, const POSE& up_lim = no_limit_up, const bool& auto_sp=1, const byte& microstepping = 1);
    ~MotorInterface(void){};

public:
// Init Pin Motors   
    void init_motors(void);

public:
// Handle Limits 
    void setLowLimit(const POSE& low_lim){_limits_pose[0]=low_lim;}
    void setUpLimit(const POSE& up_lim){_limits_pose[1]=up_lim;}
    void setLimits(const POSE& limits1, const POSE& limits2){
        _limits_pose[0] = limits1; 
        _limits_pose[1] = limits2;
    }
    void setLimits(const POSE limits[2]){setLimits(limits[0], limits[1]);}
    POSE getLowLimits(void) const {return _limits_pose[0];}
    POSE getUpLimits(void) const {return _limits_pose[1];}

public: 
// Handle Pose Goal
    bool setPoseGoal(const POSE& pose_goal){
        if (checkConsistency(pose_goal)){
            _pose_goal=pose_goal;
            _new_pose_goal = true;
            updateAllDirection();
            return true;
        }
        return false;
    }
    POSE getPoseGoal(void) const {return _pose_goal;}
    bool setNextPoseGoal(const POSE& next_pos){
        if (checkConsistency(next_pos)){
            _next_pose_goal=next_pos;
            return true;
        }
        return false;
    }
    POSE getNextPoseGoal(void) const {return _next_pose_goal;}

    byte getPoseNumber(void) const {return _pose_number;}
    void increasePoseNumber(void){_pose_number++;}
    void setPoseNumber(const byte& pose_number){_pose_number = pose_number;}
    void resetPoseNumber(void){_pose_number = 0;}

public:
// Handle Current Pose
    void setCurrentPose(const POSE& cur_pos){_current_pose=cur_pos;};
    POSE getCurrentPose(void) const {return _current_pose;}

    DIRECTION getDirection(void) const {return _direction;}
    
public:
// handle current speed
    void setCurrentSpeed(const SPEED& cur_speed){
        //_current_speed=(cur_speed<=_max_speed)?cur_speed:_max_speed;
        _slider_stepper->setSpeed(_current_speed.slider);
        _tilt_stepper->setSpeed(_current_speed.tilt);
        _pan_stepper->setSpeed(_current_speed.pan);
    }
    SPEED getCurrentSpeed(void) const {return _current_speed;}   
    void setCurrentSliderSpeed(const float& speed){_slider_stepper->setSpeed(speed);_current_speed.slider = speed;}
    void setCurrentTiltSpeed(const float& speed){_tilt_stepper->setSpeed(speed);_current_speed.tilt = speed;}
    void setCurrentPanSpeed(const float& speed){_pan_stepper->setSpeed(speed);_current_speed.pan = speed;}

// Handle Max Speed
    void setMaxSpeedSlider(const float& max_speed){
        _max_speed.slider = (max_speed <= speed_limits.slider)?max_speed:speed_limits.slider;
        _slider_stepper->setMaxSpeed(_max_speed.slider);
    }
    void setMaxSpeedTilt(const float& max_speed){
        _max_speed.tilt = (max_speed <= speed_limits.tilt)?max_speed:speed_limits.tilt;
        _tilt_stepper->setMaxSpeed(_max_speed.tilt);
    }
    void setMaxSpeedPan(const float& max_speed){
        _max_speed.pan = (max_speed <= speed_limits.pan)?max_speed:speed_limits.pan;
        _pan_stepper->setMaxSpeed(_max_speed.pan);
    }
    void setMaxSpeed(const SPEED& max_speed){
        setMaxSpeedSlider(max_speed.slider);
        setMaxSpeedTilt(max_speed.tilt);
        setMaxSpeedPan(max_speed.pan);
    }
    SPEED getMaxSpeed(void) const {return _max_speed;}

    void setAcceleration(const ACCELERATION& acc){
        _acceleration=acc;
        _slider_stepper->setAcceleration(acc.slider);
        _tilt_stepper->setAcceleration(acc.tilt);
        _pan_stepper->setAcceleration(acc.pan);
    }


private:
    void setSensorState(const SENSOR_STATE& sensors){_sensor_state=sensors;}
    SENSOR_STATE getSensorState(void) const {return _sensor_state;}

public:
    void setDelay(const unsigned long& ms_delay){_ms_delay=ms_delay;}
    unsigned long getDelay(void) const {return _ms_delay;}

    void setAutoSpeed(const bool& auto_sp){_auto_speed=auto_sp;}
    bool getAutoSpeed(void) const {return _auto_speed;}
    void setAutoMode(const bool& automode){_automode=automode;}
    bool getAutoMode(void) const {return _automode;}
    byte getMicrostepping(void) const {return _micro_stepping;}

private:
    void setManualMode(void){_manual_mode=true;}
    void unsetManualMode(void){_manual_mode=false;}

public:
    void play(void);
    void pause(void);

private:
    void updateSliderDirection(void){
        if (_current_pose.slider <= _pose_goal.slider){_direction.slider=1;}
        else{_direction.slider = 0;}
    }

    void updateSliderDirection(const short& slider_pot){
        _direction.slider = (slider_pot < 0)?0:1;
    }

    void updateTiltDirection(void){
        if (_current_pose.tilt <= _pose_goal.tilt){_direction.tilt=1;}
        else{_direction.tilt = 0;}
    }
    
    void updatePanDirection(void){
        if (_current_pose.pan <= _pose_goal.pan){_direction.pan=1;}
        else{_direction.pan = 0;}
    }
    
    void updateTiltDirection(const unsigned short& joytilt){
        if (abs((short)joytilt-512) > JOYSTICK_STOP_ZONE){
            _direction.tilt = (joytilt < 512)?0:1;
        }
    }

    void updatePanDirection(const unsigned short& joypan){
        if (abs((short)joypan-512) > JOYSTICK_STOP_ZONE){
            _direction.pan = (joypan < 512)?0:1;
        }
    }
    
    void updateAllDirection(void){
        updateSliderDirection();
        updateTiltDirection();
        updatePanDirection();
    }

private:
    bool hasLowLimitSlider(void){return true;}//!(_limits_pose[0].slider<=no_limits.slider)
    bool hasUpLimitSlider(void){return true;}//!(_limits_pose[1].slider<=no_limits.slider)
    bool isLimitedSlider(void){return (hasUpLimitSlider() && hasLowLimitSlider());}
    bool hasLowLimitTilt(void){return !(_limits_pose[0].tilt>=no_limit_low.tilt);}
    bool hasUpLimitTilt(void){return !(_limits_pose[1].tilt >=no_limit_up.tilt);}
    bool isLimitedTilt(void){return (hasUpLimitTilt() || hasLowLimitTilt());}
    bool hasLowLimitPan(void){return !(_limits_pose[0].pan  >=no_limit_low.pan);}
    bool hasUpLimitPan(void){return !(_limits_pose[1].pan   >=no_limit_up.pan);}
    bool isLimitedPan(void){return (hasUpLimitPan() && hasLowLimitPan());}

private:    
    bool checkSliderLimit(void);
    bool checkTiltLimit(void);
    bool checkPanLimit(void);
    bool checkLimits(void);

public:
    bool checkConsistency(const POSE& pose);
    bool checkConstistencySlider(const float& slider_pose);
    bool checkConsistencyTilt(const float& tilt_pose);
    bool checkConsistencyPan(const float& pan_pose);

public:
// Sensor detection and update
    void sensorDetect(void);
    bool AllSensorActive(void){
        // state = 1 no detection, = 0 detection
        return (!_sensor_state.slider && !_sensor_state.tilt && !_sensor_state.pan);
    }

private:
    bool checkSliderSensorConsistency(void);

    void needToUpdateCurrentSliderPose(const bool& slider_sensor);
    void needToUpdateCurrentTiltPose(const bool& tilt_sensor);
    void needToUpdateCurrentPanPose(const bool& pan_sensor);

    bool isNewSensorStateSlider(const bool& slider_sensor);
    bool isNewSensorStatePan(const bool& pan_sensor);
    bool isNewSensorStateTilt(const bool& tilt_sensor);
    bool AssureSensorChange(unsigned long& start_time,unsigned long& current_time, bool& cur_sens, const bool& new_sens);

private:
    void resetSliderStep(void){_step_count.slider = 0;_slider_stepper->setCurrentPosition(0);}
    void resetPanStep(void){_step_count.pan = 0;_pan_stepper->setCurrentPosition(0);}
    void resetTiltStep(void){_step_count.tilt = 0;_tilt_stepper->setCurrentPosition(0);}
    void resetAllStep(void){resetSliderStep(); resetPanStep(); resetTiltStep();}

private:
    void setCurrentSliderPoseHomePose(void);
    void setCurrentPanPoseHomePose(void);
    void setCurrentTiltPoseHomePose(void);


public:
    void setMicrostepping(const byte& microstepping);
    

private:
    void setStepMode(void);
    void writeMSPin(const bool& ms1, const bool& ms2, const bool& ms3);
    void setEnable(void);
    void setDisable(void);

private:
    int convertmmToStep(const float& slider_pos){return (slider_pos*(STEP_PER_TURN*(float)_micro_stepping))/(SLIDER_PULLEY_TEETH*PULLEY_STEP);}
    float convertStepTomm(const int& slider_step){return (slider_step*PULLEY_STEP*SLIDER_PULLEY_TEETH)/(STEP_PER_TURN*(float)_micro_stepping);}
    int convertDegreeToStepPan(const float& pan_pos){return (pan_pos*PAN_GEAR_RATIO*(float)_micro_stepping)/(DEGREE_PER_STEP);}
    float convertStepToDegreePan(const int& pan_step){return ((pan_step*DEGREE_PER_STEP)/PAN_GEAR_RATIO*(float)_micro_stepping);}
    int convertDegreeToStepTilt(const float& tilt_pos){return (tilt_pos*TILT_GEAR_RATIO*(float)_micro_stepping)/(DEGREE_PER_STEP);}
    float convertStepToDegreeTilt(const int& tilt_step){return ((tilt_step*DEGREE_PER_STEP)/TILT_GEAR_RATIO*(float)_micro_stepping);}

private:
    void computeSliderStep(void);
    void computeTiltStep(void);
    void computePanStep(void);
    void computeStep(void);

    void updateCurrentPose(void);
    void updateCurrentSliderPose(void);
    void updateCurrentTiltPose(void);
    void updateCurrentPanPose(void);

public:
    void stopSlider(void){
        _current_speed.slider = 0;
        setCurrentSliderSpeed(0);
        _slider_stepper->stop();}
    void stopTilt(void){
        _current_speed.tilt = 0;
        setCurrentTiltSpeed(0);
        _tilt_stepper->stop();}
    void stopPan(void){
        _current_speed.pan = 0;
        setCurrentPanSpeed(0);
        _pan_stepper->stop();}
    void stop(void){
        stopSlider();
        stopTilt();
        stopPan();
    }
private:
    void stopToPose(const short& slider_pot, const short& pantilt_pot);
    bool needToStopSlider(void);
    bool needToStopPan(void);
    bool needToStopTilt(void);

public:
    bool PoseGoalReached(float slider_precision = 0.5, float pan_tilt_precision = 1.5);

private:
    bool updatePoseGoal(void){
        // Serial.println(_pose_goal.slider);
        // Serial.println(_current_pose.slider);
        if (PoseGoalReached()){
            if (!(_next_pose_goal == _pose_goal))
            {
                setPoseGoal(_next_pose_goal);
            }
            return true;
        }
        return false;
    }
public:
    void run(void){
        if (_manual_mode){
            _slider_stepper->runSpeed();
            _tilt_stepper->runSpeed();
            _pan_stepper->runSpeed();
        }
        else if (_auto_speed){
            _steppers.run();
        }
        else{
            _slider_stepper->run();
            _tilt_stepper->run();
            _pan_stepper->run();
        }
    }

private:
    void interpolateSpeed(void);
    void changeSpeed(const float& max_slider_speed, const float& max_tiltpan_speed);
    bool hasChangeSpeed(const short& slider_pot, const short& pantiltpot){
        return (!((_max_speed.slider == (MAX_SPEED_STEP_MOTOR*abs((float)slider_pot))/100) && 
                (_max_speed.pan == (MAX_SPEED_STEP_MOTOR*abs((float)pantiltpot))/100)));
    }
private:    
    void convertPotToMaxSpeed(const short& slider_pot, const short& pantilt_pot);
    
    void SliderSpeedUnderThreshold(void){if (abs(_current_speed.slider)<=MIN_SPEED_STEP_MOTOR){stopSlider();}}
    void TiltSpeedUnderThreshold(void){if (abs(_current_speed.tilt)<=MIN_SPEED_STEP_MOTOR){stopTilt();}}
    void PanSpeedUnderThreshold(void){if (abs(_current_speed.pan)<=MIN_SPEED_STEP_MOTOR){stopPan();}}
    void SpeedUnderThreshold(void){
        SliderSpeedUnderThreshold();
        TiltSpeedUnderThreshold();
        PanSpeedUnderThreshold();
    }

    bool hasChangeSliderDirectionToAvoidLimits(const short& slider_pot);
    bool hasChangeTiltDirectionToAvoidLimits(const unsigned short& joytilt);
    bool hasChangePanDirectionToAvoidLimits(const unsigned short& joypan);

    void getOutSliderLimit(const short& slider_pot);
    void getOutTiltLimit(const unsigned short& joytilt);
    void getOutPanLimit(const unsigned short& joypan);

    void handleSliderTeleoperation(const short& slider_pot);
    void handleTiltTeleoperation(const short& pantilt_pot, const unsigned short& joytilt);
    void handlePanTeleoperation(const short& pantilt_pot, const unsigned short& joypan);

public:
    void teleoperation(const short& slider_pot, const short& pantilt_pot, const unsigned short& joytilt, const unsigned short& joypan);
    bool executeTrajectory(const short& slider_pot, const short& pantilt_pot);
};


void init_sensors(void);
SENSOR_STATE check_sensor(void);

#endif