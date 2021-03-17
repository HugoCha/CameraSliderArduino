#include "radio_functions.h"


void RadioMotor::initRadio(){
    pinMode(BUZZER, OUTPUT);

    _radio_motor->begin();
    _radio_motor->setDataRate(RF24_250KBPS);
    _radio_motor->setPALevel(_pa_level);
    _radio_motor->setChannel(47);
    _radio_motor->enableDynamicPayloads();
    _radio_motor->enableAckPayload();

    _radio_motor->openReadingPipe(1, thisSlaveAddress);

    _radio_motor->startListening();
    _radio_motor->writeAckPayload(1, &_slider_info, sizeof(_slider_info));
}

bool RadioMotor::checkConnection(void){
    _current_time = millis();
    if (_new_data == true || _instructions.menu.menu == WAITING_FOR_USER_CHOICE){
        _last_data_time = _current_time;
        _data_received_counter++;
        _new_data = false;
    }
    if (_data_received_counter > 10){
        setPALevel(RF24_PA_MIN);
        _data_received_counter = 0;
    }
    if (_current_time - _last_data_time > 1000){
        return noConnection();
    }
    return true;
}

bool RadioMotor::noConnection(void){
    if (_pa_level == RF24_PA_MIN){
        setPALevel(RF24_PA_MAX);
        _data_received_counter = 0;
        return true;
    }
    return false;
}

void RadioMotor::alertNoConnection(void){
    if (!checkConnection()){
        // Serial.println("No connection");
        tone(BUZZER, 1000);
    }
    else{
        noTone(BUZZER);
    }
}

void RadioMotor::receiveOption(MotorInterface& Mot){
    if ( _radio_motor->available() ) {
        _radio_motor->read( &_slider_option, sizeof(_slider_option) );
        updateLimits(true);
        setOptions(Mot);
        _new_data = true;
    }
}

void RadioMotor::updateLimits(const bool& ack){
    _limits.ack = ack;
    float float_tilt_limits[2];
    _camera->setCameraDimension(
        _slider_option.h_camera,
        _slider_option.l_camera,
        _slider_option.l_lens, 
        _slider_option.r_lens);
    _camera->computeTiltLimits(float_tilt_limits);
    _limits.tilt_limits[0] = int(float_tilt_limits[0]);
    _limits.tilt_limits[1] = int(float_tilt_limits[1]);
    _limits.slider_limits[0] = _slider_option.slider_limits[0];
    _limits.slider_limits[1] = _slider_option.slider_limits[1];
    _limits.pan_limits[0] = _slider_option.pan_limits[0];
    _limits.pan_limits[1] = _slider_option.pan_limits[1];
    _radio_motor->writeAckPayload(1, &_limits, sizeof(_limits));
}

void RadioMotor::setOptions(MotorInterface& Mot){
    if ((_slider_option.option_change/16)%2 == 1) {
        Mot.setAutoSpeed(_slider_option.auto_speed);
        saveAutoSpeed(_slider_option.auto_speed);
    }
    if ((_slider_option.option_change/8)%2 == 1) {
        Mot.setMicrostepping(_slider_option.precision);
        saveMicrostepping(_slider_option.precision);
    }
    if ((_slider_option.option_change/4)%2 == 1) {
        Mot.setAutoMode(_slider_option.auto_mode);
        saveAutomode(_slider_option.auto_mode);
    }
    if ((_slider_option.option_change/2)%2 == 1) {
        Mot.setDelay((unsigned long)_slider_option.delay*1000);
        saveDelay(_slider_option.delay);
    }
    if ((_slider_option.option_change)%2 == 1){
        POSE Low_lim = {(float)_slider_option.slider_limits[0], (float)_limits.tilt_limits[0], (float)_slider_option.pan_limits[0]};
        POSE Up_lim = {(float)_slider_option.slider_limits[1], (float)_limits.tilt_limits[1], (float)_slider_option.pan_limits[1]};
        
        Mot.setLimits(Low_lim, Up_lim);

        saveSliderLimits(_slider_option.slider_limits[0],_slider_option.slider_limits[1]);
        saveTiltLimits(_limits.tilt_limits[0],_limits.tilt_limits[1]);
        savePanLimits(_slider_option.pan_limits[0],_slider_option.pan_limits[1]);
    }
    
    if (_slider_option.active) _camera->activate();
    else _camera->desactivate();

    //Serial.println("Moteur Option : ");
    // Serial.println(Mot.getAutoSpeed());
    // Serial.println(Mot.getAutoMode());
    // Serial.println(Mot.getDelay());
    // Serial.println(Mot.getMicrostepping());
}

void RadioMotor::updateMotorInfo(const MotorInterface& Mot){
    _slider_info.pt_nb = Mot.getPoseNumber();

    _slider_info.slider_pose = Mot.getCurrentPose().slider;
    _slider_info.tilt_pose = ((int)Mot.getCurrentPose().tilt)%360;
    _slider_info.pan_pose = ((int)Mot.getCurrentPose().pan)%360;

    _slider_info.slider_speed = (abs(Mot.getCurrentSpeed().slider)*100)/Mot.getMaxSpeed().slider;
    _slider_info.tilt_speed   = (abs(Mot.getCurrentSpeed().tilt)*100)/Mot.getMaxSpeed().tilt;
    _slider_info.pan_speed    = (abs(Mot.getCurrentSpeed().pan)*100)/Mot.getMaxSpeed().pan;
}

void RadioMotor::updateBatteryInfo(Battery& bat){
    _slider_info.battery_level = bat.getBatteryPercentage();
}

bool RadioMotor::receiveTelecommand(const MotorInterface& Mot, Battery& batt){
    if ( _radio_motor->available() ) {
        uint8_t size = _radio_motor->getPayloadSize();
        _radio_motor->read( &_instructions, sizeof(_instructions) );
        
        updateSliderInfo(Mot, batt, size == sizeof(_instructions));
        _new_data = true;
    }
    return isTelecommandInstructionNew();
}


bool RadioMotor::receive( MotorInterface& Mot, Battery& batt){
    bool new_instructions = false; // if the instruction changed
    if (_data2receive == 0){ // If the data received will be struct telecommand
        new_instructions = receiveTelecommand(Mot, batt);
        if (_instructions.menu.menu == 0)
            _data2receive = 1;
    }
    else if (_data2receive == 1){ 
        receiveOption(Mot);
        if (_new_data) {_data2receive = 0;delay(10);}
        
    }
    alertNoConnection();
    return new_instructions;
}