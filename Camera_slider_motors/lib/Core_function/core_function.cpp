#include "core_function.h"

/***************************************************
 * 
 * Parser
 * 
***************************************************/

void InstructionHandler::parseTelecommandInstructions(void){
    parseState();
    parseMenu();
}


void InstructionHandler::parseMenu(){
    if (_play){
        switch(_instructions.menu.menu){
            case FIND_HOME_MODE:
                findHomeMode();
            break;
            case MANUAL_MODE:
                manualMode();
            break;
            case REGISTER_NUMERIC_MODE:
                registerNumericMode();
            break;
            case PROGRAMMED_MODE:
                programmedMode(_motor->getAutoMode());
            break;
            case REGISTER_TRACKING_MODE:
                registerTrackingMode();
            break;
            case TIMELAPSE:
                timelapseMode();
            break;
            case REGISTER_HYPERLPSE:
                registerHyperlapseMode();
            break;
            case HYPERLAPSE:
                hyperlapseMode();
            break;
            case PANORAMICLAPSE:
                panoramiclapseMode();
            break;
            case RESET:
                resetAll();
            break;
            default:
            break;
        }
    }
}

void InstructionHandler::parseState(void){
    switch (_instructions.state){
    case 's':
        stop();
    break;
    case 'r':
        play();
    break;
    default:
        pause();
        break;
    }
}

/**********************************************************
 * 
 * Play/Pause/stop
 * 
***********************************************************/

void InstructionHandler::play(void){
    _play =true;
    _motor->play();
}

void InstructionHandler::pause(void){
    _play = false;
    _motor->pause();
}

void InstructionHandler::stop(void){
    resetAll();
    pause();
}


/****************************************************************
 * 
 * Add pose, clear last pose, clear all pose
 * 
****************************************************************/
void InstructionHandler::addPose(const int newpose[3]){
    addPose({(float)newpose[0], (float)newpose[1], (float)newpose[2]});
}

void InstructionHandler::addPose(const POSE& newpose){
    _target_pose[_number_of_pose].slider = newpose.slider;
    _target_pose[_number_of_pose].tilt = newpose.tilt;
    _target_pose[_number_of_pose].pan = newpose.pan;
    _number_of_pose++;
}


void InstructionHandler::clearLastPose(void){
    _target_pose[_number_of_pose-1].slider = 0;
    _target_pose[_number_of_pose-1].tilt = 0;
    _target_pose[_number_of_pose-1].pan = 0;
    _number_of_pose--;
}

void InstructionHandler::clearAllPose(void){
    for (int i=_number_of_pose; i>0; i--)
        clearLastPose();
}

void InstructionHandler::resetAll(void){
    clearAllPose();
    clearTrackingPose();
    _number_of_photo = 0;
    _current_pose_index = 0;
    _play = false;
    _start_time = _current_time = millis();
    _start = false;
}

/**************************************************************
 * 
 * Register mode
 * 
***************************************************************/

void InstructionHandler::registerNumericMode(void){
    if (_instructions.menu.pose_number >= _number_of_pose && 
    _instructions.menu.pose_number!=255)
    { // If user add pose
        addPose({_instructions.menu.PointA[0], _instructions.menu.PointA[1], _instructions.menu.PointA[2]});
    }
    // for (int i=0; i<_number_of_pose; i++){
    //     Serial.println();
    //     Serial.print("Target Pose Array : ");
    //     Serial.print("S : ");
    //     Serial.print(_target_pose[i].slider);
    //     Serial.print(" T : ");
    //     Serial.print(_target_pose[i].tilt);
    //     Serial.print(" P : ");
    //     Serial.println(_target_pose[i].pan);
    //}
}



/***************************************************************
 * 
 * Manual Mode
 * 
****************************************************************/

void InstructionHandler::manualMode(void){
    motorTeleop();
}

void InstructionHandler::motorTeleop(void){
    _motor->teleoperation(
        (short)_instructions.PotentiometerX-100,
        (short)_instructions.PotentiometerPT,
        _instructions.JoyX, 
        _instructions.JoyY
    );
}

/****************************************************************
 * 
 * Programmed mode/Automode
 * 
*****************************************************************/

bool InstructionHandler::programmedMode(void){
    if (!_start){
        _motor->resetPoseNumber();
        _motor->setPoseGoal(_target_pose[0]);
        _motor->setNextPoseGoal(_target_pose[1]);
        _start = true;
        _current_pose_index = 2;
        // Serial.println(_target_pose[0].slider);
        // Serial.println(_target_pose[0].tilt);
        // Serial.println(_target_pose[0].pan);
        // Serial.println(_target_pose[1].slider);
        // Serial.println(_target_pose[1].tilt);
        // Serial.println(_target_pose[1].pan);
    }
    if (motorExec()){
        _motor->increasePoseNumber();
        _camera->shoot();
        if (_current_pose_index > _number_of_pose) {return true;}
        else if (_current_pose_index == _number_of_pose) {_current_pose_index++;}
        else {_motor->setNextPoseGoal(_target_pose[_current_pose_index++]);
        }
    }

    return false;
}


void InstructionHandler::autoMode(void){
    static bool reverse = false;
    bool exec;
    if (!_start){
        _motor->setPoseGoal(_target_pose[0]);
        _motor->setNextPoseGoal(_target_pose[1]);
        _current_pose_index = 2;
        _start = true;
    }
    exec = motorExec();
    if (exec && !reverse){
        _motor->setNextPoseGoal(_target_pose[_current_pose_index++]);
        if (_current_pose_index >= _number_of_pose){
            reverse = true;
            _current_pose_index = _number_of_pose-1;        
        }  
    }
    else if (exec && reverse){
        Serial.println();
        _motor->setNextPoseGoal(_target_pose[--_current_pose_index]);
        if (_current_pose_index <= 0){
            reverse = false;
            _current_pose_index = 1;
        }

    }
}


bool InstructionHandler::motorExec(void){
    return (
    _motor->executeTrajectory(
        (short)_instructions.PotentiometerX,
        (short)_instructions.PotentiometerPT
    ));
}

bool InstructionHandler::programmedMode(const bool& automode){
    if (automode)
        autoMode();
    else
        return programmedMode();
    return false;
}

/*****************************************************************
 * 
 * Find Home
 * 
******************************************************************/


bool InstructionHandler::findHomeMode(void)
{
    _motor->setPoseGoal(_motor->_home_pose1);
    
    if (_motor->executeTrajectory(50, 50) && _motor->AllSensorActive())
        return true;
    return false;
}



/*****************************************************************
 * 
 * Interpolation
 * 
*****************************************************************/


void InstructionHandler::interpolationTracking(
    const short object_pose[3], 
    const unsigned short& start_slider, 
    const unsigned short& end_slider)
{
    float start_pan_angle = computePanAngleFromPose(object_pose[1]*10, object_pose[0]*10, start_slider);
    float end_pan_angle = computePanAngleFromPose(object_pose[1]*10, object_pose[0]*10, end_slider);

    float tilt_angle = computeTiltAngleFromPose(object_pose[2]*10, object_pose[1]*10);

    addPose({start_slider, tilt_angle, start_pan_angle});
    addPose({end_slider, tilt_angle, end_pan_angle});
}


float InstructionHandler::computePanAngleFromPose(
    const short& object_pose_opposed_length, 
    const short& object_pose_adjacent_length, 
    const unsigned short& camera_pose_slider)
{
    // *10 because object pose in cm
    float angle = (object_pose_adjacent_length==camera_pose_slider)?90.0:atan2(object_pose_opposed_length, abs((float)camera_pose_slider-object_pose_adjacent_length))*180/PI;
    if (object_pose_opposed_length >= 0)
        return (abs(object_pose_adjacent_length)<camera_pose_slider)?(180.0-angle):angle;

    return (abs(object_pose_adjacent_length)<camera_pose_slider)?(-180.0-angle):angle;

}

float InstructionHandler::computeTiltAngleFromPose(
    const short& object_pose_opposed_length, 
    const short& object_pose_adjacent_length)
{
    // *10 because object pose in cm
    return atan2(object_pose_opposed_length-(PLATFORM_HEIGHT + _camera->getCameraHeight()/2), abs(object_pose_adjacent_length))*180/PI;
}


void InstructionHandler::registerTrackingMode(void){
    Serial.println(_instructions.menu.PointA[0]);
    if (_instructions.menu.pose_number==0){
        _object_pose[0] = _instructions.menu.PointA[0];
        _object_pose[1] = _instructions.menu.PointA[1];
        _object_pose[2] = _instructions.menu.PointA[2];
    }
    else{
        if (_instructions.menu.pose_number > _number_of_pose)
            addPose(_instructions.menu.PointA);
    }
    if  (_number_of_pose == 2){
        unsigned short start_slider = _target_pose[0].slider;
        unsigned short end_slider = _target_pose[1].slider;
        for (int i=0; i<_number_of_pose; i++){
            Serial.println();
            Serial.print("Target Pose Array : ");
            Serial.print("S : ");
            Serial.print(_target_pose[i].slider);
            Serial.print(" T : ");
            Serial.print(_target_pose[i].tilt);
            Serial.print(" P : ");
            Serial.println(_target_pose[i].pan);
        }
        clearAllPose();
        interpolationTracking(_object_pose, start_slider, end_slider);
    }
}

/*****************************************************************
 * 
 * Timelapse/Hyperlapse
 * 
 *****************************************************************/

bool InstructionHandler::timelapseMode(void){
    if (!_start){
        _motor->setPoseGoal({
            (float)_instructions.menu.PointA[0],
            (float)_instructions.menu.PointA[1],
            (float)_instructions.menu.PointA[2]
        });
        _number_of_photo = _instructions.menu.pose_number;
        while (!_motor->executeTrajectory(100, 100));
        _start_time = _current_time = millis();
    }
    else{
        _current_time = millis();
        if (_current_time-_start_time > _instructions.menu.ms_delay && _current_pose_index < _number_of_photo){
            _camera->shoot();
            _current_pose_index++;
            _start_time = _current_time;
        }
    }
    return (_current_pose_index == _number_of_photo);
}


void InstructionHandler::registerHyperlapseMode(void){
    addPose(_instructions.menu.PointA);
    if (_number_of_pose == 2){
        _start_pose = _target_pose[0];
        _end_pose = _target_pose[1];
    }
}

void InstructionHandler::interpolationHyperlapse(const POSE& start_pose, const POSE& end_pose, int& number_of_photo_left){
    
    POSE pose2add = _target_pose[_number_of_pose-2];
    if (pose2add == start_pose){
        pose2add.slider -= (end_pose.slider - start_pose.slider)/(_number_of_photo);
        pose2add.tilt   -= (end_pose.tilt - start_pose.tilt)/(_number_of_photo);
        pose2add.pan    -= (end_pose.pan - start_pose.pan)/(_number_of_photo);
    }
    //else{++number_of_photo_left;}

    int number_of_pose2add = (number_of_photo_left+1+_number_of_photo/(TARGET_ARRAY_SIZE) < TARGET_ARRAY_SIZE)?number_of_photo_left+1+_number_of_photo/(TARGET_ARRAY_SIZE):TARGET_ARRAY_SIZE;
    
    clearAllPose();
    
    for (int i=0; i<number_of_pose2add;i++){
        pose2add.slider += (end_pose.slider - start_pose.slider)/(_number_of_photo);
        pose2add.tilt   += (end_pose.tilt - start_pose.tilt)/(_number_of_photo);
        pose2add.pan    += (end_pose.pan - start_pose.pan)/(_number_of_photo);
        addPose(pose2add);
        number_of_photo_left--;
    }
}

bool InstructionHandler::hyperlapseMode(void){
    static int number_of_photo_left = _number_of_photo;
    
    if (!_start || _current_pose_index == TARGET_ARRAY_SIZE+1){
        interpolationHyperlapse(_start_pose, _end_pose, number_of_photo_left);
        _start = false;
        //number_of_photo_left -= (TARGET_ARRAY_SIZE-_number_of_photo/(TARGET_ARRAY_SIZE));
    }
    
    return (programmedMode() && number_of_photo_left<=0 );
}


/******************************************************************
 * 
 * PanoramicLapse
 * 
*******************************************************************/

bool InstructionHandler::panoramiclapseMode(void){}