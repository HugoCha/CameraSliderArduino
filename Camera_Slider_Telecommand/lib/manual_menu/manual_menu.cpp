#include "manual_menu.h"

ManualMenu::~ManualMenu(){}

bool ManualMenu::execute()
{
    lcd->clear();
    //if(enterManualMode()){
    if (display_instructions()==1){
        play();
        while (handleManualMode()){
            handleDisplay();
        }
    }
    return true;
}


bool ManualMenu::handleManualMode(void){
    updateButtons();
    handleButtons();
    updateInstructions();
    sendInstructions();
    return !_stop;
}

/**************************************************************
 * 
 * Buttons
 * 
**************************************************************/

void ManualMenu::handleButtons(void)
{
    handleEnterButton();
    handleReturnButton();
}


void ManualMenu::handleEnterButton(void)
{
    if (_pause && !_buttons_state.enter_button)
        play();
    else if (!_buttons_state.enter_button && !_last_buttons_state.enter_button)
        _radio_telecommand->setShutter(true);
    else
        _radio_telecommand->setShutter(false);
}


/*****************************************************************
 * 
 * Radio Instruction/send
 * 
******************************************************************/


void ManualMenu::updateInstructions(void)
{
    _radio_telecommand->setMenu(MANUAL_MODE);
    _radio_telecommand->setJoystick(_buttons_state.JoyX, _buttons_state.JoyY);
    _radio_telecommand->setPotentiometer(_buttons_state.potentiometerX, _buttons_state.potentiometerPT);
    _radio_telecommand->setFocus(_buttons_state.focus_button && !_last_buttons_state.focus_button);
}


/*****************************************************************************
 * 
 * Display
 * 
*****************************************************************************/

int ManualMenu::display_instructions(){
    phi_prompt_struct myLongMsg;

    myLongMsg.ptr.msg_P=manual_instructions; // Assign the address of the text string to the pointer.
    myLongMsg.low.i=1; // Default text starting position. 0 is highly recommended.
    myLongMsg.high.i=strlen_P(manual_instructions); // Position of the last character in the text string, which is size of the string - 1.
    myLongMsg.step.c_arr[0]=get_lcd_row()-1; // rows to auto fit entire screen
    myLongMsg.step.c_arr[1]=get_lcd_col()-2; // one col list
    myLongMsg.col=0; // Display the text area starting at column 0
    myLongMsg.row=1; // Display the text area starting at row 0
    myLongMsg.option=1; // Option 0, display classic message, option 1, display message with scroll bar on right.++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

    lcd->clear();

    display_title(true, true, false);

    return text_area_P(&myLongMsg);
}


void ManualMenu::display_motor_info(void)
{
    display_motor_info(
        _radio_telecommand->getSliderPose(),
        _radio_telecommand->getTiltPose(),
        _radio_telecommand->getPanPose(),
        _radio_telecommand->getSliderSpeed(),
        _radio_telecommand->getTiltSpeed(),
        _radio_telecommand->getPanSpeed()
    );
}


void ManualMenu::display_motor_info(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Vx, const unsigned short& VangleY, const unsigned short& VangleZ){
    
    lcd->clear();
    display_title(false, true, false);

    update_display_motor_info(posX, angleY, angleZ, Vx, VangleY, VangleZ);

    lcd->setCursor(0,3);
    lcd->print("OK:Shoot     <:Pause");
}

void ManualMenu::update_display_motor_info(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Vx, const unsigned short& VangleY, const unsigned short& VangleZ)
{
    display_3digits_number_with_char(lcd, "X", posX, 0, 1);

    display_3digits_number_with_char(lcd, "T", angleY, 7, 1, 1);

    display_3digits_number_with_char(lcd, "P", angleZ, 14, 1, 1);
    
    display_3digits_number_with_char(lcd, "U", Vx, 0, 2, 0);
    lcd->setCursor(5,2);
    lcd->print((char)0x25);

    display_3digits_number_with_char(lcd, "V", VangleY, 7, 2, 0);
    lcd->setCursor(12,2);
    lcd->print((char)0x25);
    
    display_3digits_number_with_char(lcd, "W", VangleZ, 14, 2, 0);
    lcd->setCursor(19,2);
    lcd->print((char)0x25);
}


void ManualMenu::update_display_motor_info(void)
{
    update_display_motor_info(
        (int)_radio_telecommand->getSliderPose(),
        (int)_radio_telecommand->getTiltPose(),
        (int)_radio_telecommand->getPanPose(),
        (int)_radio_telecommand->getSliderSpeed(),
        (int)_radio_telecommand->getTiltSpeed(),
        (int)_radio_telecommand->getPanSpeed()
    );
}