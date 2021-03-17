#include "camera_functions.h"

void Camera::init_camera(void){
    pinMode(SHUTTER, OUTPUT);
    pinMode(FOCUS, OUTPUT);
}

bool Camera::shoot(void){
    if (_active){
        static unsigned long start_shoot_time = 0, current_shoot_time = 0;
        if (!_shoot){
            digitalWrite(SHUTTER, HIGH);
            _shoot = true;
            start_shoot_time = current_shoot_time = millis();
        }
        else{
            if (current_shoot_time - start_shoot_time > SHUTTER_DELAY){
                digitalWrite(SHUTTER, LOW);
                current_shoot_time = start_shoot_time = 0;
                _shoot = false;
                return true;
            }
            current_shoot_time = millis();
        }
    }
    else{
        digitalWrite(SHUTTER, LOW);
    }
    return false;
}


bool Camera::focus(void){
    if (_active){
        static unsigned long start_focus_time = 0, current_focus_time = 0;
        if (!_focus){
            digitalWrite(FOCUS, HIGH);
            _shoot = true;
            start_focus_time = current_focus_time = millis();
        }
        else{
            if (current_focus_time - start_focus_time > FOCUS_DELAY){
                digitalWrite(FOCUS, LOW);
                current_focus_time = start_focus_time = 0;
                _focus = false;
                return true;
            }
            current_focus_time = millis();
        }
    }
    else{
        digitalWrite(SHUTTER, LOW);
    }
    return false;
}


void Camera::computeTiltLimits(float limits[2], const float& margin){

    if (_r_lens > 0 && _l_lens > 0 && (_l_lens+CAMERA_PLATFORM_LENGTH/2.0) >= CAMERA_PLATFORM_HEIGHT){
        float a = (CAMERA_PLATFORM_LENGTH/2.0 + _l_lens);
        float b = (_r_lens - (CAMERA_PLATFORM_RADIUS - _h_camera/2.0));
        float lens_vector_norm = sqrt(a*a + b*b);
        
        float sol1 = acos((CAMERA_PLATFORM_HEIGHT)/lens_vector_norm) - atan2(a, b);
        if (0<=abs(sol1) && abs(sol1)<=PI/2){limits[0] = ((_direction)?(abs(sol1)+margin):-(abs(sol1)+margin))*180/PI;}
        else{
            sol1 = (-acos((CAMERA_PLATFORM_HEIGHT)/lens_vector_norm) - atan2(a, b));
            limits[0] = ((_direction)?(abs(sol1)+margin):-(abs(sol1)+margin))*180/PI;
        }
    }

    else{limits[0] = 180;}

    if (_h_camera > 0 && _h_camera-CAMERA_PLATFORM_RADIUS >= CAMERA_PLATFORM_HEIGHT){
        float a = CAMERA_PLATFORM_LENGTH/2.0;
        float b = - (_h_camera - CAMERA_PLATFORM_RADIUS);
        float top_camera_vector_norm = sqrt(a*a + b*b);
        float sol2 = acos((CAMERA_PLATFORM_HEIGHT)/top_camera_vector_norm) - atan2(a, b);
        
        if (PI/2<=abs(sol2) && abs(sol2)<=PI){limits[1] = ((_direction)?(abs(sol2)+margin):-(abs(sol2)+margin))*180/PI;}
        else{
            sol2 = -acos((CAMERA_PLATFORM_HEIGHT)/top_camera_vector_norm) - atan2(a, b);
            limits[1] = ((_direction)?(abs(sol2)+margin):-(abs(sol2)+margin))*180/PI;
        }
    }
    
    else{limits[1] = 180;}
}