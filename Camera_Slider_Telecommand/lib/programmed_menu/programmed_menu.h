#ifndef __PROGRAMMED_MENU_H__
#define __PROGRAMMED_MENU_H__

#include <LiquidCrystal_I2C.h>

#include "phi_interfaces.h"
#include "phi_prompt.h"
#include "lcd_functions.h"
#include "menu_class.h"

#define MAX_NB_POINT 15

// const char PROGMEM pr_sub_menu_item00[]="Enr. manuelle";
const char PROGMEM pr_sub_menu_item01[]="Enr. numerique";
const char PROGMEM pr_sub_menu_item02[]="Parcours enregistre";
const char PROGMEM pr_sub_menu_item03[]="Tracking";
//const char PROGMEM * const pr_sub_menu_items[] = {pr_sub_menu_item00, pr_sub_menu_item01, pr_sub_menu_item02, pr_sub_menu_item03};
const char PROGMEM * const pr_sub_menu_items[] = {pr_sub_menu_item01, pr_sub_menu_item02, pr_sub_menu_item03};

struct CrossingPoint{
    uint8_t point_nb;
    int slider_pos;
    int tilt_pos;
    int pan_pos;
};

typedef struct CrossingPoint CROSSPOINT;

//const char PROGMEM manual_instructions[]="You can control the plateform with the joystick and the speed of the plateform and rail with potentiometers.\n Click OK to continue.";
// const char PROGMEM program_instructions1[]=
// "Vous pouvez controler le support avec le joystick, \
// vous pouvez aussi controlez la vitesse \
// du support et du rail avec les potentiometres. \
// Vous pouvez rentrer des points de passage en cliquant sur >. \
// Cliquez sur OK pour continuer.";
const char PROGMEM program_instructions2[]=
"Vous pouvez rentrer des points de passages, si vous connaissez \
les points de passage au prealable.\
Cliquez sur OK pour continuer.";
const char PROGMEM program_instructions3[]=
"Vous faire le tracking d'un objet en entrant sa position et\
les positions de depart et d'arrivee du slider. \
Cliquer OK pour continuer.";
// const char PROGMEM program_instructions4[]=
// "Vous pouvez suivre le dernier parcours qui a ete enregistre.
// Cliquer OK pour continuer.";

void init_sub_menu(LiquidCrystal_I2C* lcd, phi_prompt_struct& myMenu, const uint8_t& lcd_c, const uint8_t& lcd_r);
void programmed_sub_menu(LiquidCrystal_I2C* lcd, const uint8_t& lcd_c, const uint8_t& lcd_r, RadioTelecommand* radio_tel);


class ProgrammedMenu : public MenuClass{

protected:
    byte _current_index = 0;
    uint8_t _number_of_point=0;
    uint8_t _size_of_array=0;
    CROSSPOINT _crosspointarray[MAX_NB_POINT];

private:
    byte _programm=255; 
    // 0 : numeric record, 
    // 1 : manual record,
    // 2 : tracking,
    // 3 : execute traj
    // 4 : record traj


public:
    ProgrammedMenu( LiquidCrystal_I2C * screen, uint8_t col, uint8_t row, RadioTelecommand* radio_tel) : 
    MenuClass(screen, col, row, radio_tel,""){
        for (uint8_t i=0; i<MAX_NB_POINT;++i)
            _crosspointarray[i] = {i, 0, 0, 0}; 
        _size_of_array=0;
    }
    ~ProgrammedMenu(){};
    
    bool enterProgrammedMode(const int& menu);
    bool quitProgrammedMode(void){return _stop;}
    bool handleProgrammedMode(void);

protected:
    uint8_t getNbPoint(void) const {return _number_of_point;}
    void setNbPoint(uint8_t pt){_number_of_point = pt;}

private:
    void setProgramm(byte pr){
        // if (pr == 0) _programm = REGISTER_MANUAL_MODE;
        if (pr == 0) _programm = REGISTER_NUMERIC_MODE;
        //if (pr == 2) _programm = REGISTER_TRAJECTORY;
        if (pr == 1) _programm = REGISTER_TRACKING_MODE;
        //if (pr == 4) _programm = REGISTER_MANUAL_MODE;
    }
    byte getProgramm(void) const {return _programm;}

private:
    void enterNumberOfPoint(void);

protected:
    int display_instructions() override;

    void display_crossing_pt_nb(const uint8_t& pt_nb);
    void display_pt(const uint8_t& pt_nb);
    void display_object_pose(void);
    void display_slider_pose(const byte& pose);
    void display_motor_info(void) override;
    void display_motor_info(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Vx, const unsigned short& VangleY, const unsigned short& VangleZ) override;
    void display_motor_info_execute(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Pot_x, const unsigned short& Pot_Pt);
    void display_sub_menu_title(void);
    void initRecordDisplay(void);

    void update_display_motor_info(void) override;
    void update_display_motor_info(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Vx, const unsigned short& VangleY, const unsigned short& VangleZ) override;
    

    bool record(void);
    bool execute(void) override;
    
    void updateButtonsExecute(void);
    void handleButtons(void);
    
    void handleEnterButton(void);

    bool manualRecord(void);
    bool numericRecord(void);
    void enterNumericPoint(void);
    // bool recordTraj(void);
    bool recordTracking(void);

    void updatePoint2Send(const int& nb);
    bool sendAllPoses(void);
    void updateInstructions(void) override;

    void add_point(const int& slider_pos, const int& tilt_pos, const int& pan_pos);
    void pop_point(void);
    void clear(void);

private:
    bool actionNumericRecord(const int& temp, const int intput_nb_arr[3]);
};

#endif