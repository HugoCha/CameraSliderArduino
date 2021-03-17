#ifndef __CAMERA_FUNCTIONS_H__
#define __CAMERA_FUNCTIONS_H__

//#include "configurations.h"
#include <Arduino.h>

#define SHUTTER 12
#define FOCUS 13

// Geometric constant of the platform
#define CAMERA_PLATFORM_HEIGHT 80.0 // TO MEASURE height between axes of camera support and the PCB
#define CAMERA_PLATFORM_RADIUS 47.5
#define CAMERA_PLATFORM_LENGTH 60.0

#define SHUTTER_DELAY 200 // To test
#define FOCUS_DELAY 50 // To test


class Camera{
private:
    bool _shoot = false;
    bool _focus = false;
    int _h_camera;
    int _l_lens;
    int _r_lens;
    bool _direction;
    bool _active;

public:
    Camera(const int& h_camera=0, const int& l_lens=0, const int& r_lens=0, const bool& direction=0, const bool& active=true) :
    _h_camera(h_camera),  _l_lens(l_lens), _r_lens(r_lens), _direction(direction), _active(active){}
    ~Camera(){}

    void activate(void){_active=true;init_camera();}
    void desactivate(void){_active=false;}

    void setCameraDimension(const int& hc, const int& ll, const int& rl){
        _h_camera = hc;
        _l_lens = ll;
        _r_lens = rl;
    }
    void setCameraHeight(const int& h){_h_camera=h;}
    void setLensLength(const int& l){_l_lens=l;}
    void setRadiusLength(const int& r){_r_lens=r;}
    void setDirection(const bool& dir){_direction=dir;}

    int getCameraHeight(void) const {return _h_camera;}

    void computeTiltLimits(float limits[2], const float& margin=0.17);// margin = 10°

    bool shoot(void);
    bool focus(void);

    void init_camera(void);
};

#endif