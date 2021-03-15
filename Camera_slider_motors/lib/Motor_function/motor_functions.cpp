#include "motor_functions.h"

/************************************************
 * 
 * Sensors
 * 
 * **********************************************/


void init_sensors(void){
    pinMode(IR_SLIDER, INPUT);
    pinMode(HALL_TILT, INPUT);
    pinMode(HALL_PAN, INPUT);
    return;
}


SENSOR_STATE check_sensor(void){
    SENSOR_STATE sensors;
    sensors.slider = digitalRead(IR_SLIDER);
    sensors.tilt   = digitalRead(HALL_TILT);
    sensors.pan    = digitalRead(HALL_PAN);
    return sensors;
}

void MotorInterface::sensorDetect(void){
    SENSOR_STATE cur_sensor = check_sensor();
    needToUpdateCurrentSliderPose(cur_sensor.slider);
    needToUpdateCurrentTiltPose(cur_sensor.tilt);
    needToUpdateCurrentPanPose(cur_sensor.pan);
}

bool MotorInterface::checkSliderSensorConsistency(void){
    // Check if slider sensor is Consistent compared with current pose
    // If the current pose computed is not close to one of home pose
    if (!((_home_pose1.slider-10<=_current_pose.slider && _current_pose.slider<=_home_pose1.slider+10))||
        (_home_pose2.slider-10<=_current_pose.slider && _current_pose.slider<=_home_pose2.slider+10)){
            _slider_obstacle = true;
            return false;
        }
    return true;
}


void MotorInterface::needToUpdateCurrentSliderPose(const bool& slider_sensor){
    if (isNewSensorStateSlider(slider_sensor)){
        if (!_sensor_state.slider){ // If an obstacle is detected boolean inverted
            if (checkSliderSensorConsistency()){
                setCurrentSliderPoseHomePose();
                computeSliderStep();
            }
        }
        else if (_sensor_state.slider && _slider_obstacle){
            _slider_obstacle = false;
            computeSliderStep();
        }
    }
    else if (_slider_obstacle){
        _slider_stepper->stop();
    }
}

void MotorInterface::needToUpdateCurrentTiltPose(const bool& tilt_sensor){
    if (isNewSensorStateTilt(tilt_sensor)){
        if (!_sensor_state.tilt){
            setCurrentTiltPoseHomePose();
            computeTiltStep();
        }
    }
}

void MotorInterface::needToUpdateCurrentPanPose(const bool& pan_sensor){
    if (isNewSensorStateTilt(pan_sensor)){
        if (!_sensor_state.pan){
            setCurrentPanPoseHomePose();
            computePanStep();
        }
    }
}

bool MotorInterface::AssureSensorChange(unsigned long& start_time, unsigned long& current_time, bool& cur_sens, const bool& new_sens){
    // If a change of state has occured and it's sure that's not a sensor error 
    if (cur_sens != new_sens && 
    current_time - start_time >= SENSOR_CHANGE_MIN_TIME ){
        cur_sens = new_sens;
        start_time = current_time = 0;
        return true;
    }
    // If a change of state has occured and it's not sure that's not a sensor error 
    else if (cur_sens != new_sens){
        current_time = micros();
    }
    // If no change has occured or the sensor is back to normal value after error
    else{
        start_time = current_time = 0;
    }
    return false;
}


bool MotorInterface::isNewSensorStateSlider(const bool& slider_sensor){
    static unsigned long start_time_ir_slider = 0, current_time_ir_slider = 0;

    return AssureSensorChange(
        start_time_ir_slider, 
        current_time_ir_slider, 
        _sensor_state.slider, 
        slider_sensor);
}


bool MotorInterface::isNewSensorStatePan(const bool& pan_sensor){
    static unsigned long start_time_hall_pan = 0, current_time_hall_pan = 0;

    return AssureSensorChange(
        start_time_hall_pan, 
        current_time_hall_pan, 
        _sensor_state.pan, 
        pan_sensor);
}


bool MotorInterface::isNewSensorStateTilt(const bool& tilt_sensor){
    static unsigned long start_time_hall_tilt = 0, current_time_hall_tilt = 0;

    return AssureSensorChange(
    start_time_hall_tilt, 
    current_time_hall_tilt, 
    _sensor_state.tilt, 
    tilt_sensor);
}


/************************************************
 * 
 * CONSTRUCTORS
 * 
 * **********************************************/


MotorInterface::MotorInterface(AccelStepper* slider_step, AccelStepper* tilt_step, AccelStepper* pan_step, const ACCELERATION& acceleration, const POSE& low_lim, const POSE& up_lim,const bool& auto_sp, const byte& microstepping){
    _slider_stepper = slider_step;
    _pan_stepper = pan_step;
    _tilt_stepper = tilt_step;
    _slider_stepper->setCurrentPosition(0);
    _tilt_stepper->setCurrentPosition(0);
    _pan_stepper->setCurrentPosition(0);

    _steppers.addStepper(*_slider_stepper);
    _steppers.addStepper(*_tilt_stepper);
    _steppers.addStepper(*_pan_stepper);
    

    setLimits(low_lim, up_lim);
    _auto_speed = auto_sp;
    setMicrostepping(microstepping);

    _pose_goal = {0, 0.0, 0.0};
    //_current_pose = {0, 0.0, 0.0};
    _next_pose_goal = {0, 0.0, 0.0};

    _current_speed = {0, 0, 0};
    _max_speed = {0, 0, 0};
    //setAcceleration(acceleration);

    _step_count = {0, 0, 0};
    _direction = {0, 0, 0};
    _sensor_state = {0, 0, 0};
    _ms_delay = 0;
    _manual_mode = false;
}


MotorInterface::MotorInterface(
    AccelStepper* steppers[3], 
    const ACCELERATION& acceleration, 
    const POSE& low_lim, 
    const POSE& up_lim,
    const bool& auto_sp, 
    const byte& microstepping) :
    MotorInterface(
        steppers[0], 
        steppers[1], 
        steppers[2], 
        acceleration, 
        low_lim,
        up_lim, 
        auto_sp, 
        microstepping){}



/***************************************************
 * 
 * Initialisation
 * 
 * ************************************************/


void MotorInterface::init_motors(void){
    pinMode(ENABLE, OUTPUT);
    pinMode(MS1, OUTPUT);
    pinMode(MS2, OUTPUT);
    pinMode(MS3, OUTPUT);

    pinMode(STEP1, OUTPUT);
    pinMode(DIR1, OUTPUT);

    pinMode(STEP2, OUTPUT);
    pinMode(DIR2, OUTPUT);

    pinMode(STEP3, OUTPUT);
    pinMode(DIR3, OUTPUT);

    setEnable();
    setStepMode();
}


void MotorInterface::setMicrostepping(const byte& microstepping){
    _micro_stepping = microstepping;
    setStepMode();   
}


void MotorInterface::setStepMode(void){
    switch (_micro_stepping){
        case 2: // HALF STEP
            writeMSPin(1, 0, 0);
        break;
        case 4: // QUARTER STEP
            writeMSPin(1, 0, 0);
        break;
        case 8: // EIGHTH STEP
            writeMSPin(1, 1, 0);
        case 16:
            writeMSPin(1, 1, 1);
        default: // FULL STEP
            writeMSPin(0, 0, 0);
        break;
    }
}


void MotorInterface::writeMSPin(const bool& ms1, const bool& ms2, const bool& ms3){
    digitalWrite(MS1, ms1);
    digitalWrite(MS2, ms2);
    digitalWrite(MS3, ms3);
}


void MotorInterface::setEnable(void){
    digitalWrite(ENABLE, LOW);
}

void MotorInterface::setDisable(void){
    digitalWrite(ENABLE, HIGH);
}

/***************************************************
 * 
 * Limits
 * 
 * ************************************************/


bool MotorInterface::checkConsistency(const POSE& pose){
    return (
        checkConsistencyPan(pose.pan) && 
        checkConsistencyTilt(pose.tilt) && 
        checkConstistencySlider(pose.slider)
    );
}

bool MotorInterface::checkConstistencySlider(const float& slider_pose){
    return (slider_pose >= _limits_pose[0].slider && slider_pose <= _limits_pose[1].slider);
}

bool MotorInterface::checkConsistencyTilt(const float& tilt_pose){
    bool ok=1;
    if (hasLowLimitTilt()){
        if (!(tilt_pose >= _limits_pose[0].tilt && tilt_pose<= 180-_limits_pose[0].tilt)){ok=0;}
    }
    if (hasUpLimitTilt()){
        if ((tilt_pose > _limits_pose[1].tilt && tilt_pose > 0) || (tilt_pose < 0 && tilt_pose < -_limits_pose[1].tilt)){ok=0;}
    }
    return ok;
}


bool MotorInterface::checkConsistencyPan(const float& pan_pose){
    bool ok=1;
    if (hasLowLimitPan()){
        if (!(pan_pose >= _limits_pose[0].pan && pan_pose <= 180-_limits_pose[0].pan)){ok=0;}
    }
    if (hasUpLimitPan()){
        if ((pan_pose > _limits_pose[1].pan && pan_pose < -_limits_pose[1].pan)){ok=0;}
    }
    return ok;
}

bool MotorInterface::checkSliderLimit(void){
    return checkConstistencySlider(_current_pose.slider);
}
bool MotorInterface::checkTiltLimit(void){
    return checkConsistencyTilt(_current_pose.tilt);
}
bool MotorInterface::checkPanLimit(void){
    return checkConsistencyPan(_current_pose.pan);
}
bool MotorInterface::checkLimits(void){
    return checkConsistency(_current_pose);
}

/*****************************************************
 * 
 * Stop to limit Handler
 * 
 * ****************************************************/

bool MotorInterface::needToStopSlider(void){
    if ((_manual_mode && !checkSliderLimit()) || _slider_obstacle){
        stopSlider();
        return true;
    }
    return false;
}

bool MotorInterface::needToStopTilt(void){
    if (!checkTiltLimit()){
        stopTilt();
        return true;
    }
    return false;
}

bool MotorInterface::needToStopPan(void){
    if (!checkPanLimit()){
        stopPan();
        return true;
    }
    return false;
}

/*************************************************
 * 
 * Reset Pose
 * 
 * **********************************************/

void MotorInterface::setCurrentSliderPoseHomePose(void){
    _current_pose.slider = (_direction.slider)?_home_pose2.slider : _home_pose1.slider;
}


void MotorInterface::setCurrentPanPoseHomePose(void){
    _current_pose.pan = (_direction.pan)?_home_pose2.pan : _home_pose1.pan;
}


void MotorInterface::setCurrentTiltPoseHomePose(void){
    _current_pose.tilt = (_direction.tilt)?_home_pose2.tilt : _home_pose1.tilt;
}


/*****************************************************
 * 
 * Compute Step
 * 
 * ***************************************************/

void MotorInterface::computeSliderStep(void){
    resetSliderStep();
    _slider_stepper->moveTo(convertmmToStep(_pose_goal.slider - _current_pose.slider));
}


void MotorInterface::computeTiltStep(void){
    resetTiltStep();
    _tilt_stepper->moveTo(convertDegreeToStepTilt(_pose_goal.tilt - _current_pose.tilt));
}


void MotorInterface::computePanStep(void){
    resetPanStep();
    _pan_stepper->moveTo(convertDegreeToStepPan(_pose_goal.pan - _current_pose.pan));
}


void MotorInterface::computeStep(void){
    computeSliderStep();
    computeTiltStep();
    computePanStep();
}


void MotorInterface::updateCurrentSliderPose(void){
    long curr_step = _slider_stepper->currentPosition();
    _current_pose.slider += convertStepTomm(curr_step-_step_count.slider);
    _step_count.slider = curr_step;
}


void MotorInterface::updateCurrentTiltPose(void){
    long curr_step = _tilt_stepper->currentPosition();
    _current_pose.tilt += convertStepToDegreeTilt(curr_step-_step_count.tilt);
    _step_count.tilt = curr_step;
}


void MotorInterface::updateCurrentPanPose(void){
    long curr_step = _pan_stepper->currentPosition();
    _current_pose.pan += convertStepToDegreePan(curr_step-_step_count.pan);
    _step_count.pan = curr_step;
}


void MotorInterface::updateCurrentPose(void){
    updateCurrentSliderPose();
    updateCurrentTiltPose();
    updateCurrentPanPose();
}

/*****************************************************
 * 
 * Interpolate Speed
 * 
 * ***************************************************/


void MotorInterface::interpolateSpeed(void){
    if (_auto_speed){
        long stepToGoSlider = convertmmToStep(_pose_goal.slider-_current_pose.slider);
        long stepToGoTilt   = convertDegreeToStepTilt(_pose_goal.tilt-_current_pose.tilt);
        long stepToGoPan    = convertDegreeToStepPan(_pose_goal.pan-_current_pose.pan);
        long stepToGo[3]    = {stepToGoSlider, stepToGoTilt, stepToGoPan};
        
        resetAllStep();
        
        if (stepToGoSlider != 0){
            _steppers.moveTo(stepToGo);
        }
        else if (stepToGoPan!=0 || stepToGoTilt!=0){
            setMaxSpeedTilt(_max_speed.slider);
            setMaxSpeedPan(_max_speed.slider);
            _steppers.moveTo(stepToGo);
        }
        else{
            stop();
        }
        _current_speed.slider = _slider_stepper->speed();
        _current_speed.tilt = _tilt_stepper->speed();
        _current_speed.pan = _pan_stepper->speed();
    }
    return;
}


void MotorInterface::convertPotToMaxSpeed(const short& slider_pot, const short& pantilt_pot){
    if (hasChangeSpeed(slider_pot, pantilt_pot) || _new_pose_goal){
        if (_manual_mode){
            updateSliderDirection(slider_pot);
            setMaxSpeed({
                abs((float)slider_pot)*(MAX_SPEED_STEP_MOTOR/100),
                abs((float)pantilt_pot)*(MAX_SPEED_STEP_MOTOR/100),
                abs((float)pantilt_pot)*(MAX_SPEED_STEP_MOTOR/100),
            });
        }
        else{
            changeSpeed(abs((float)slider_pot)*(MAX_SPEED_STEP_MOTOR/100),
                        abs((float)pantilt_pot)*(MAX_SPEED_STEP_MOTOR/100));
        }
    }
}

void MotorInterface::changeSpeed(const float& max_slider_speed, const float& max_tiltpan_speed){
    if (_auto_speed){
        setMaxSpeed({max_slider_speed, MAX_SPEED_STEP_MOTOR, MAX_SPEED_STEP_MOTOR});
        interpolateSpeed();
    }
    else{
        setMaxSpeed({max_slider_speed, max_tiltpan_speed, max_tiltpan_speed});
        setCurrentSpeed(_max_speed);
    }
}

/*************************************************************
 * 
 * 
 * Play/Pause Handle
 * 
 * ***********************************************************/

void MotorInterface::play(){
    _pause = false;
}

void MotorInterface::pause(){
    _pause=true;
    stop();
}


/*************************************************************
 * 
 * Teleoperation
 * 
 * ***********************************************************/

bool MotorInterface::hasChangeSliderDirectionToAvoidLimits(const short& slider_pot){
    if ((_current_pose.slider <= _limits_pose[0].slider && slider_pot > 0) ||
        (_current_pose.slider >= _limits_pose[1].slider && slider_pot < 0))
    {
        return true;
    }
    return false;
}

bool MotorInterface::hasChangeTiltDirectionToAvoidLimits(const unsigned short& joytilt){
    updateTiltDirection(joytilt);
    if (hasLowLimitTilt() && hasUpLimitTilt()){
        if ((((_current_pose.tilt <= _limits_pose[0].tilt) && joytilt > 512+JOYSTICK_STOP_ZONE) ||
            ((_current_pose.tilt >= 180-_limits_pose[0].tilt) && joytilt < 512-JOYSTICK_STOP_ZONE)) || 
            (((_current_pose.tilt >= _limits_pose[1].tilt) && joytilt < 512-JOYSTICK_STOP_ZONE) ||
            ((_current_pose.tilt <= -_limits_pose[1].tilt) && joytilt > 512+JOYSTICK_STOP_ZONE)))
            {
                return true;
            }
    else if (hasLowLimitTilt()){
        if (((_current_pose.tilt <= _limits_pose[0].tilt) && joytilt > 512+JOYSTICK_STOP_ZONE) ||
            ((_current_pose.tilt >= 180-_limits_pose[0].tilt) && joytilt < 512-JOYSTICK_STOP_ZONE))
            {
                return true;
            }
        }
    }
    else{
        if (((_current_pose.tilt >= _limits_pose[1].tilt) && joytilt < 512-JOYSTICK_STOP_ZONE) ||
            ((_current_pose.tilt <= 360+_limits_pose[1].tilt) && joytilt > 512+JOYSTICK_STOP_ZONE))
            {
                return true;
            }
    }
    
    return false;
}

bool MotorInterface::hasChangePanDirectionToAvoidLimits(const unsigned short& joypan){
    updatePanDirection(joypan);
    if (hasLowLimitPan() && hasUpLimitPan()){
        if ((((_current_pose.pan <= _limits_pose[0].pan) && joypan > 512+JOYSTICK_STOP_ZONE) ||
            ((_current_pose.pan >= 180-_limits_pose[0].pan) && joypan < 512-JOYSTICK_STOP_ZONE)) || 
            (((_current_pose.pan >= _limits_pose[1].pan) && joypan < 512-JOYSTICK_STOP_ZONE) ||
            ((_current_pose.pan <= -_limits_pose[1].pan) && joypan > 512+JOYSTICK_STOP_ZONE)))
            {
                return true;
            }
    else if (hasLowLimitPan()){
        if (((_current_pose.pan <= _limits_pose[0].pan) && joypan > 512+JOYSTICK_STOP_ZONE) ||
            ((_current_pose.pan >= 180-_limits_pose[0].pan) && joypan < 512-JOYSTICK_STOP_ZONE))
            {
                return true;
            }
        }
    }
    else{
        if (((_current_pose.pan >= _limits_pose[1].pan) && joypan < 512-JOYSTICK_STOP_ZONE) ||
            ((_current_pose.pan <= 360+_limits_pose[1].pan) && joypan > 512+JOYSTICK_STOP_ZONE))
            {
                return true;
            }
    }
    
    return false;
}

void MotorInterface::getOutSliderLimit(const short& slider_pot){
    if (hasChangeSliderDirectionToAvoidLimits(slider_pot)){
        _current_speed.slider = ((float)slider_pot/100) * MAX_SPEED_STEP_MOTOR;
        //_direction.slider = !_direction.slider;
        //_pose_goal.slider = (_direction.slider)?1000:-1000;
    }
    else{
        stopSlider();
    }
}

void MotorInterface::getOutTiltLimit(const unsigned short& joytilt){
    if (hasChangeTiltDirectionToAvoidLimits(joytilt)){
        _current_speed.tilt = (((float)joytilt-512)/512) * _max_speed.tilt;
        //_direction.tilt = !_direction.tilt;
        //_pose_goal.tilt = (_direction.tilt)?1000:-1000;
    }
    else{
        stopTilt();
    }
}


void MotorInterface::getOutPanLimit(const unsigned short& joypan){
    if (hasChangeTiltDirectionToAvoidLimits(joypan)){
        _current_speed.pan = (((float)joypan-512)/512) * _max_speed.pan;
        //_direction.pan = !_direction.pan;
        //_pose_goal.pan = (_direction.pan)?1000:-1000;
    }
    else{
        stopPan();
    }
}

void MotorInterface::handleSliderTeleoperation(const short& slider_pot){
    _max_speed.slider = MAX_SPEED_STEP_MOTOR;
    if (!(needToStopSlider())){
        updateSliderDirection(slider_pot);
        _current_speed.slider = (((float)slider_pot)/100)*MAX_SPEED_STEP_MOTOR;
        //_pose_goal.slider = (_direction.slider)?-1000:+1000;
    }
    else{
        if (!_slider_obstacle){
            getOutSliderLimit(slider_pot);
        }
    }
}

void MotorInterface::handleTiltTeleoperation(const short& pantilt_pot, const unsigned short& joytilt){
    _max_speed.tilt = ((float)pantilt_pot * MAX_SPEED_STEP_MOTOR)/100.0;
    if (!needToStopTilt()){
        //updateTiltDirection(joytilt);
        _current_speed.tilt = 10*int(((((float)joytilt-512)/512)*_max_speed.tilt)/10);
    }
    else{
        getOutTiltLimit(joytilt);
    }
}

void MotorInterface::handlePanTeleoperation(const short& pantilt_pot, const unsigned short& joypan){
    _max_speed.pan = ((float)pantilt_pot * MAX_SPEED_STEP_MOTOR)/100;
    if (!needToStopPan()){
        _current_speed.pan = (((float)joypan-512)/512)*_max_speed.pan;
        updatePanDirection(joypan);
        //_pose_goal.pan = (_direction.pan)?-1000:1000;
    }
    else{
        getOutPanLimit(joypan);
    }
}

void MotorInterface::teleoperation(const short& slider_pot, const short& pantilt_pot, const unsigned short& joytilt, const unsigned short& joypan){
    setManualMode();
    sensorDetect();
    updateCurrentPose();
    if (!_pause){

        handleSliderTeleoperation(slider_pot);
        handleTiltTeleoperation(pantilt_pot, joytilt);
        handlePanTeleoperation(pantilt_pot, joypan);
        
        setMaxSpeed(_max_speed);
        setCurrentSpeed(_current_speed);
        SpeedUnderThreshold();
    }
}


/*****************************************************
 * 
 * Trajectory Execution
 * 
 * ***************************************************/

bool MotorInterface::PoseGoalReached(float slider_precision, float pan_tilt_precision){
    return (IS_IN(_current_pose.slider, _pose_goal.slider, slider_precision) &&
            IS_IN(_current_pose.tilt, _pose_goal.tilt, pan_tilt_precision) &&
            IS_IN(_current_pose.pan, _pose_goal.pan, pan_tilt_precision));
}


void MotorInterface::stopToPose(const short& slider_pot, const short& pantilt_pot){
    static unsigned long start_time_pause=0, current_time_pause=0;
    if (_new_pose_goal && current_time_pause - start_time_pause <= _ms_delay){
        stop();
        current_time_pause = millis();
    }
    else{
        convertPotToMaxSpeed(slider_pot, pantilt_pot);
        start_time_pause = current_time_pause = millis();
        _new_pose_goal = false;
    }
} 


bool MotorInterface::executeTrajectory(const short& slider_pot, const short& pantilt_pot){
    unsetManualMode();
    //sensorDetect();
    updateCurrentPose();
    if (!_pause){
        stopToPose(slider_pot, pantilt_pot);
        //convertPotToMaxSpeed(slider_pot, pantilt_pot);    
    }
    needToStopSlider();
    return (updatePoseGoal());
}