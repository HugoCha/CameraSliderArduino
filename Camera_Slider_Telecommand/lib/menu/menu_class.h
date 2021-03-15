#ifndef __MENU_CLASS_H__
#define __MENU_CLASS_H__

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "phi_interfaces.h"
#include "phi_prompt.h"
#include "lcd_functions.h"
#include "radio_function.h"

#define SIGN(a) ((a>0)?1:-1)

#define JOYX A0
#define JOYY A1
#define POT_X A2
#define POT_PT A3
#define ENTER 2
#define RETURN 3
#define FOCUS 4

#define WAITING_TIME 10
#define DEBOUNCE_DELAY 100


typedef struct buttons
{
    int JoyX=0;
    int JoyY=0;
    byte potentiometerX=0;
    byte potentiometerPT=0;
    bool return_button=false;
    bool enter_button=false;
    bool focus_button=false;
} TELECOMMAND_BUTTONS;

class MenuClass{
protected:
    LiquidCrystal_I2C * lcd;
private:
    uint8_t lcd_col;
    uint8_t lcd_row;  
      
protected:
    RadioTelecommand* _radio_telecommand;
    bool _pause = false;
    bool _start = true;
    bool _stop = false;
    bool _ack = true;
    char _last_screen='s';
    unsigned long _start_time=0;
    TELECOMMAND_BUTTONS _buttons_state;
    TELECOMMAND_BUTTONS _last_buttons_state;


    String title;
    

public:
    MenuClass(LiquidCrystal_I2C * screen, uint8_t col, uint8_t row, RadioTelecommand* radio_tel,String t):
    lcd(screen), lcd_col(col), lcd_row(row), _radio_telecommand(radio_tel),title(t){};
    virtual ~MenuClass();

public:
    virtual bool execute() = 0;

protected:
    uint8_t get_lcd_col() const {return lcd_col;}
    uint8_t get_lcd_row() const {return lcd_row;}

    void setTitle(String t){title = t;}
    String getTitle(void) const {return title;}

    
    virtual int display_instructions() = 0;
    virtual void display_motor_info(void) = 0;
    virtual void display_motor_info(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Vx, const unsigned short& VangleY, const unsigned short& VangleZ) = 0;
    virtual void update_display_motor_info(void) = 0;
    virtual void update_display_motor_info(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Vx, const unsigned short& VangleY, const unsigned short& VangleZ) = 0;
    virtual void updateInstructions(void) = 0;
    
    virtual void sendInstructions(void);
    virtual void updateButtons(void);
    virtual void updateEnterButton(void);
    virtual void updateReturnButton(void);
    virtual void updateFocusButton(void);
    virtual void handleReturnButton(void);

    virtual void play(void);
    virtual void pause(void);
    virtual void stop(void);

    virtual void handleDisplay(void);
    virtual void display_title(const bool& first, const bool& last, const bool& bad_connction);
    virtual void display_pause();
    virtual void display_bad_connection();
    virtual void debounceButton(const bool& read_value, bool& last_state, bool& current_state, unsigned long& last_debounce_time);
};

#endif