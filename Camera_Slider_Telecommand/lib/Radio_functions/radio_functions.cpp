#include "radio_function.h"

void RadioTelecommand::initRadio(void){
    while(!_radio_telecommand->begin());
    _radio_telecommand->setDataRate(RF24_250KBPS);
    _radio_telecommand->setPALevel(_pa_level);
    _radio_telecommand->setChannel(47);
    _radio_telecommand->enableDynamicPayloads();
    _radio_telecommand->enableAckPayload();
    _radio_telecommand->setRetries(5,15);

    _radio_telecommand->openWritingPipe(thisSlaveAddress);

    _radio_telecommand->stopListening();
}


bool RadioTelecommand::checkConnection(void){
    _current_time = millis();
    if (_new_data == true){
        _last_data_time = _current_time;
        _ack_received++;
        _new_data = false;
    }
    if (_ack_received > 40){
        setPALevel(RF24_PA_MIN);
        _ack_received = 1;
    }
    if (_current_time - _last_data_time > 1000){
        return noConnection();
    }
    return true;
}

bool RadioTelecommand::noConnection(void){
    if (_pa_level == RF24_PA_MIN){
        setPALevel(RF24_PA_MAX);
        _ack_received = 0;
        return true;
    }
    return false;
}


bool RadioTelecommand::sendTelecommand(void){
    
    bool rslt = _radio_telecommand->write( &_instructions, sizeof(_instructions) );

    if (rslt && _radio_telecommand->isAckPayloadAvailable() ) {
        receiveSliderInfo();
    }
    checkConnection();
    return _ack_received;
}


bool RadioTelecommand::sendOption(void){
    _instructions.menu.menu = OPTION_SEND;
    
    if (sendTelecommand()){
        return writeOption();
    }
    return false;
}

bool RadioTelecommand::writeOption(void){
    bool rslt = _radio_telecommand->write( &_slider_option, sizeof(_slider_option) );

    if (rslt && _radio_telecommand->isAckPayloadAvailable() ) {
        receiveLimits();
    }
    checkConnection();
    return _ack_received;
}


void RadioTelecommand::receiveSliderInfo(void){
    _radio_telecommand->read(&_slider_info, sizeof(_slider_info));
    _new_data = true;
}



void RadioTelecommand::receiveLimits(void){
    _radio_telecommand->read(&_limits, sizeof(_limits));
    _new_data = true;
}
