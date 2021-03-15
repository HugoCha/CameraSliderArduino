#include "programmed_menu.h"


/*******************************************************************************
 * 
 * Sub Menu Programmed Mode
 * 
*******************************************************************************/


void programmed_sub_menu(LiquidCrystal_I2C* lcd, const uint8_t& lcd_c, const uint8_t& lcd_r, RadioTelecommand* radio_tel){
    //int menu_pointer_1=0; // This stores the menu choice the user made.
    phi_prompt_struct myMenu; // This structure stores the main menu.
    init_sub_menu(lcd, myMenu, lcd_c, lcd_r);
    
    ProgrammedMenu* PM = new ProgrammedMenu(lcd, lcd_c, lcd_r, radio_tel);
    
    while(1)
    {
        select_list(&myMenu);
        if (PM->enterProgrammedMode(myMenu.low.i)){
            while (!PM->handleProgrammedMode());
        }
        if (PM->quitProgrammedMode()){
            break;
        }
    }
    delete PM;
    return;
}



void init_sub_menu(LiquidCrystal_I2C* lcd, phi_prompt_struct& myMenu, const uint8_t& lcd_c, const uint8_t& lcd_r){

    // Initialize the top menu
    myMenu.ptr.list=(char**)&pr_sub_menu_items; // Assign the list to the pointer
    myMenu.low.i=0; // Default item highlighted on the list
    myMenu.high.i=3; // Last item of the list is size of the list - 1.
    myMenu.width=lcd_c-1; // Length in characters of the longest list item.
    myMenu.step.c_arr[0]=3;//lcd_rows-1; // rows to auto fit entire screen
    myMenu.step.c_arr[1]=1; // one col list
    myMenu.step.c_arr[2]=0; // y for additional feature such as an index
    myMenu.step.c_arr[3]=lcd_c-5; // x for additional feature such as an index
    myMenu.col=0; // Display prompt at column 0
    myMenu.row=1; // Display prompt at row 1
    myMenu.option=45;

    lcd->clear();  // Refresh menu if a button has been pushed
    lcd->print("Parcours Programme");
}


/*************************************************************************
 * 
 * Enter/Quit Programmed Mode
 * 
*************************************************************************/

bool ProgrammedMenu::enterProgrammedMode(const int& i)
{   
    setProgramm(i);
    int awnser = display_instructions();
    if (awnser == 1){    
        _stop = false;
        return true;
    }
    else if (awnser == -1){
        _stop = true;
    }
    return false;
}


/************************************************************************
 * 
 * Handle Programmed Mode
 * 
*************************************************************************/

bool ProgrammedMenu::handleProgrammedMode(void)
{
    while(record() && !_stop);
    while(sendAllPoses() && !_stop);
    while(execute() && !_stop);
    return true;
}

/*************************************************************************
 * 
 * Handle Displays
 * 
*************************************************************************/

void ProgrammedMenu::display_sub_menu_title(void)
{
    // if (_programm == REGISTER_MANUAL_MODE)
    //     this->setTitle("Enr. Mann.");
    if (_programm == REGISTER_NUMERIC_MODE)
        this->setTitle("Enr. Num.");
    else if (_programm == REGISTER_TRAJECTORY)
        this->setTitle("Enr. Traj.");
    else if (_programm == REGISTER_TRACKING_MODE)
        this->setTitle("Tracking");
    else if (_programm == PROGRAMMED_MODE)
        this->setTitle("Execute Traj.");
    
    
    lcd->clear();
    this->display_title(false, false, false);
    
    return;
}

int ProgrammedMenu::display_instructions()
{
    phi_prompt_struct myLongMsg1;
    switch(_programm){
        // case REGISTER_MANUAL_MODE:
        //     myLongMsg1.ptr.msg_P=program_instructions1; // Assign the address of the text string to the pointer.
        //     myLongMsg1.high.i=strlen_P(program_instructions1);
        // break;
        case REGISTER_NUMERIC_MODE:
            myLongMsg1.ptr.msg_P=program_instructions2; // Assign the address of the text string to the pointer.
            myLongMsg1.high.i=strlen_P(program_instructions2); // Position of the last character in the text string, which is size of the string - 1.
        break;
        // case REGISTER_TRAJECTORY:
        //     myLongMsg1.ptr.msg_P=program_instructions3; // Assign the address of the text string to the pointer.
        //     myLongMsg1.high.i=strlen_P(program_instructions3); // Position of the last character in the text string, which is size of the string - 1.
        case REGISTER_TRACKING_MODE:
            myLongMsg1.ptr.msg_P=program_instructions3; // Assign the address of the text string to the pointer.
            myLongMsg1.high.i=strlen_P(program_instructions3); // Position of the last character in the text string, which is size of the string - 1.
        default:
        break;
    }
    
    myLongMsg1.low.i=0; // Default text starting position. 0 is highly recommended.
    myLongMsg1.step.c_arr[0]=get_lcd_row()-1; // rows to auto fit entire screen
    myLongMsg1.step.c_arr[1]=get_lcd_col()-2; // one col list
    myLongMsg1.col=0; // Display the text area starting at column 0
    myLongMsg1.row=1; // Display the text area starting at row 0
    myLongMsg1.option=1; // Option 0, display classic message, option 1, display message with scroll bar on right.++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!

    this->display_sub_menu_title();
    return text_area_P(&myLongMsg1);
}

void ProgrammedMenu::display_crossing_pt_nb(const uint8_t& pt_nb){
    display_pt(pt_nb);

    lcd->setCursor(0, 2);
    display_3digits_number_with_char(lcd, "X", _crosspointarray[pt_nb].slider_pos, 0, 2);
    display_3digits_number_with_char(lcd, "T", _crosspointarray[pt_nb].tilt_pos, 7, 2, 1);
    display_3digits_number_with_char(lcd, "P", _crosspointarray[pt_nb].pan_pos, 14, 2, 1);
}

void ProgrammedMenu::display_object_pose(void)
{
    display_sub_menu_title();
    lcd->setCursor(0, 1);
    lcd->print("Object Pose (cm) :");
    
    display_3digits_number_with_char(lcd, "X", _crosspointarray[0].slider_pos, 0, 2);
    display_3digits_number_with_char(lcd, "Y", _crosspointarray[0].tilt_pos, 7, 2, 1);
    display_3digits_number_with_char(lcd, "Z", _crosspointarray[0].pan_pos, 14, 2, 1);
}

void ProgrammedMenu::display_slider_pose(const byte& pose){
    display_sub_menu_title();
    lcd->setCursor(0, 1);
    if (pose == 1)
        lcd->print("Initial Pose (mm):");
    else
        lcd->print("Final Pose (mm):");
    
    display_3digits_number_with_char(lcd, "X", _crosspointarray[0].slider_pos, 0, 2);
}

void ProgrammedMenu::display_pt(const uint8_t& pt_nb){
    lcd->setCursor(0, 1);
    lcd->print("Pt:");
    lcd->setCursor(3, 1);
    lcd->print("  ");
    lcd->setCursor(3,1);
    lcd->print(pt_nb);
}

void ProgrammedMenu::display_motor_info(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Vx, const unsigned short& VangleY, const unsigned short& VangleZ)
{
    lcd->clear();
    display_sub_menu_title();

    update_display_motor_info(posX, angleY, angleZ, Vx, VangleY, VangleZ);

    lcd->setCursor(0,3);
    lcd->print("OK:Register  <:Pause");
}

void ProgrammedMenu::display_motor_info_execute(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Pot_X, const unsigned short& Pot_Pt)
{
    lcd->clear();
    display_sub_menu_title();

    update_display_motor_info(posX, angleY, angleZ, Pot_Pt, Pot_X, 0);

    lcd->setCursor(0,3);
    lcd->print("OK:Shoot     <:Pause");
}


void ProgrammedMenu::update_display_motor_info(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Vx, const unsigned short& VangleY, const unsigned short& VangleZ)
{
    display_pt(_current_index);

    display_3digits_number_with_char(lcd, "U", Vx, 7, 1, 0);
    lcd->setCursor(12,1);
    lcd->print((char)0x25);
    
    display_3digits_number_with_char(lcd, "V", VangleY, 14, 1, 0);
    lcd->setCursor(19,1);
    lcd->print((char)0x25);
    
    display_3digits_number_with_char(lcd, "X", posX, 0, 2);

    display_3digits_number_with_char(lcd, "T", angleY, 7, 2, 1);

    display_3digits_number_with_char(lcd, "P", angleZ, 14, 2, 1);
}

void ProgrammedMenu::display_motor_info(void)
{
    if (_programm == PROGRAMMED_MODE){
        display_motor_info_execute(
            (int)_radio_telecommand->getSliderPose(),
            (int)_radio_telecommand->getTiltPose(),
            (int)_radio_telecommand->getPanPose(),
            (int)_buttons_state.potentiometerX,
            (int)_buttons_state.potentiometerPT
        );
    }
    else{
        display_motor_info(
            (int)_radio_telecommand->getSliderPose(),
            (int)_radio_telecommand->getTiltPose(),
            (int)_radio_telecommand->getPanPose(),
            (int)_buttons_state.potentiometerX,
            (int)_buttons_state.potentiometerPT,
            0
        );
    }
}


void ProgrammedMenu::update_display_motor_info(void)
{
    int potX = (_programm == REGISTER_MANUAL_MODE)?_buttons_state.potentiometerX-100:_buttons_state.potentiometerX;
    _current_index = (_programm == PROGRAMMED_MODE)?_radio_telecommand->getSliderInfo().pt_nb:_size_of_array;
    update_display_motor_info(
        (int)_radio_telecommand->getSliderPose(),
        (int)_radio_telecommand->getTiltPose(),
        (int)_radio_telecommand->getPanPose(),
        potX,
        _buttons_state.potentiometerPT,
        0
    );
}


void ProgrammedMenu::enterNumberOfPoint(void)
{
    int nb_pt = 0;
    
    lcd->setCursor(3,1);
    lcd->print("Number of pts");
    
    enter_number(lcd, nb_pt, 2, 10, 2, 0, 0, MAX_NB_POINT);
    _number_of_point = (uint8_t)nb_pt;
    return;
}


void ProgrammedMenu::initRecordDisplay(void){
    lcd->clear();
    display_sub_menu_title();
    enterNumberOfPoint();
    lcd->clear();
    display_sub_menu_title();
}


/**********************************************************************************
 * 
 * Add/Pop/Clear Point from crossing point array
 * 
***********************************************************************************/


void ProgrammedMenu::add_point(const int& slider_pos, const int& tilt_pos, const int& pan_pos)
{
    if (_size_of_array <= _number_of_point){
        _crosspointarray[_size_of_array] = {_size_of_array, slider_pos, tilt_pos, pan_pos};
        _size_of_array++;
    }
    return;
}



void ProgrammedMenu::pop_point(void)
{
    _crosspointarray[_size_of_array] = {_size_of_array, 0, 0, 0};
    if (_size_of_array>0)
        _size_of_array--;
    return;
}


void ProgrammedMenu::clear(void){
    while(_size_of_array>0){
        pop_point();
    }
    return;
}

/***********************************************************************
 * 
 * Handle Buttons
 * 
***********************************************************************/

void ProgrammedMenu::handleButtons(void){
    handleEnterButton();
    handleReturnButton();
}


void ProgrammedMenu::handleEnterButton(void)
{
    if (_pause && !_buttons_state.enter_button)
        play();
    else if (!_buttons_state.enter_button && _last_buttons_state.enter_button && _ack)
    {
        if (_programm == REGISTER_MANUAL_MODE){
            add_point(
            _radio_telecommand->getSliderPose(),
            _radio_telecommand->getTiltPose(),
            _radio_telecommand->getPanPose());
        }
        else if (_programm == PROGRAMMED_MODE){
            _radio_telecommand->setShutter(true);
        }
    }
    else
        _radio_telecommand->setShutter(false);
}




/***********************************************************************
 * 
 * Record
 * 
************************************************************************/

bool ProgrammedMenu::record(){

    updateInstructions();
    sendInstructions();
    // if (_programm == REGISTER_MANUAL_MODE)
    //     return manualRecord();
    if (_programm == REGISTER_NUMERIC_MODE)
        return numericRecord();
    // else if (_programm == REGISTER_TRAJECTORY)
    //     return recordTraj();
    else if (_programm == REGISTER_TRACKING_MODE)
        return recordTracking();
    return true;
}



/***********************************************************************
 * 
 * Manual Record
 * 
***********************************************************************/

// bool ProgrammedMenu::manualRecord(void)
// {
//     if (!_number_of_point){
//         initRecordDisplay();
//         play();
//     }
//     handleDisplay();
//     updateButtons();
//     handleButtons();

//     if (!_number_of_point) stop();
//     return (!(_size_of_array == _number_of_point));
// }


/******************************************************************
 * 
 * Numeric Record
 * 
******************************************************************/

bool ProgrammedMenu::numericRecord(void){
    int temp = 0;
    uint8_t nb_size[3] = {3, 3, 3};  
    uint8_t sign[3] = {0, 1, 1};
    int min_number[3] = {
        _radio_telecommand->getLowLimitSlider(),
        _radio_telecommand->getLowLimitTilt(),
        _radio_telecommand->getLowLimitPan()};
    int max_number[3] = {
        _radio_telecommand->getUpLimitSlider(),
        _radio_telecommand->getUpLimitTilt(),
        _radio_telecommand->getUpLimitPan()};
    int input_nb_arr[3]= {_crosspointarray[0].slider_pos,
                           _crosspointarray[0].tilt_pos,
                           _crosspointarray[0].pan_pos};

    
    initRecordDisplay();
    display_crossing_pt_nb(0);
    

    while (_size_of_array < _number_of_point)
    {    
        temp = enter3number(lcd, input_nb_arr, nb_size, 2, sign, min_number, max_number);
        if (!actionNumericRecord(temp, input_nb_arr)){return true;}
        input_nb_arr[0] = _crosspointarray[_size_of_array].slider_pos;
        input_nb_arr[1] = _crosspointarray[_size_of_array].tilt_pos;
        input_nb_arr[2] = _crosspointarray[_size_of_array].pan_pos;
    }
    
    lcd->clear();
    if (!_number_of_point) stop();
    return false;
}


bool ProgrammedMenu::actionNumericRecord(const int& temp, const int intput_nb_arr[3]){
    //crosspointarray->point_nb = size_of_array;
    switch(temp){
        case -2:
            stop();
            return false;
        break;
        case -1:
            if (_size_of_array == 0){
                stop();
                return false;
            }
            pop_point();
            display_crossing_pt_nb(_size_of_array);
        break;
        case 1:
            add_point(intput_nb_arr[0], intput_nb_arr[1], intput_nb_arr[2]);
            display_crossing_pt_nb(_size_of_array);
        break;
        default:
        break;
    }
    return true;
}


/******************************************************************
 * 
 * Trajectory Record
 * 
******************************************************************/


// bool ProgrammedMenu::recordTraj(void){
//     return false;
// }

/***********************************************************************
 * 
 * Record Tracking
 * 
***********************************************************************/

bool ProgrammedMenu::recordTracking(){
    int temp = 0;
    uint8_t nb_size[3] = {3, 3, 3};  
    uint8_t sign[3] = {0, 1, 1};
    int min_number[3] = {-1,-1,-1};
    int max_number[3] = {-1,-1,-1};
    int input_nb_arr[3]= {_crosspointarray[0].slider_pos,
                           _crosspointarray[0].tilt_pos,
                           _crosspointarray[0].pan_pos};

    _number_of_point = 3;
    display_object_pose();

    while (_size_of_array < _number_of_point)
    {
        if (_size_of_array == 0){
            display_object_pose();  
            temp = enter3number(lcd, input_nb_arr, nb_size, 2, sign, min_number, max_number);
        }
        else{
            display_slider_pose(_size_of_array);
            enter_number(lcd, input_nb_arr[0], 3, 2, 2, 0, _radio_telecommand->getLowLimitSlider(), _radio_telecommand->getUpLimitSlider());
        }
        if (!actionNumericRecord(temp, input_nb_arr)){return true;}
        input_nb_arr[0] = _crosspointarray[_size_of_array].slider_pos;
        input_nb_arr[1] = _crosspointarray[_size_of_array].tilt_pos;
        input_nb_arr[2] = _crosspointarray[_size_of_array].pan_pos;
    }
    lcd->clear();
    return false;
}

/************************************************************************
 * 
 * Trajectory Execution
 * 
************************************************************************/

void ProgrammedMenu::updateButtonsExecute(void){
    _buttons_state.potentiometerX = map(analogRead(POT_X), 0, 856, 0, 100);
    _buttons_state.potentiometerPT = map(analogRead(POT_PT), 0, 1023, 0, 100);
    updateEnterButton();
    updateFocusButton();
    updateReturnButton();
}


bool ProgrammedMenu::execute(void){
    updateButtonsExecute();
    handleButtons();
    updateInstructions();
    sendInstructions();
    handleDisplay();
    if (_current_index > _number_of_point) stop();
    return (_current_index <= _number_of_point);
}


/**************************************************************************
 * 
 * Update/send Instructions
 * 
***************************************************************************/

void ProgrammedMenu::updateInstructions(void)
{
    if (_number_of_point == 0){
        _radio_telecommand->setMenu(WAITING_TIME);
    }
    else{
        if (_programm == REGISTER_MANUAL_MODE) _radio_telecommand->setMenu(MANUAL_MODE);
        else if (_programm == REGISTER_NUMERIC_MODE) _radio_telecommand->setMenu(REGISTER_NUMERIC_MODE);
        else if (_programm == REGISTER_TRAJECTORY) _radio_telecommand->setMenu(REGISTER_TRAJECTORY);
        else if (_programm == REGISTER_TRACKING_MODE) _radio_telecommand->setMenu(REGISTER_TRACKING_MODE);
        else if (_programm == PROGRAMMED_MODE) _radio_telecommand->setMenu(PROGRAMMED_MODE);
        else _radio_telecommand->setMenu(WAITING_TIME);
    }
    _radio_telecommand->setJoystick(_buttons_state.JoyX, _buttons_state.JoyY);
    _radio_telecommand->setPotentiometer(_buttons_state.potentiometerX, _buttons_state.potentiometerPT);
    _radio_telecommand->setFocus(_buttons_state.focus_button && !_last_buttons_state.focus_button);
}


void ProgrammedMenu::updatePoint2Send(const int& nb){
    _radio_telecommand->setPoseNumber(nb);
    _radio_telecommand->setNextSliderPoint(_crosspointarray[nb].slider_pos);
    _radio_telecommand->setNextTiltPoint(_crosspointarray[nb].tilt_pos);
    _radio_telecommand->setNextPanPoint(_crosspointarray[nb].pan_pos);
}



bool ProgrammedMenu::sendAllPoses(void)
{
    int nb = 0;
    _programm = (_programm==REGISTER_MANUAL_MODE)?REGISTER_NUMERIC_MODE:_programm;
    play();
    updateInstructions();
    updatePoint2Send(0);
    while (nb < _size_of_array){
        sendInstructions();
        updateButtons();
        handleButtons();
        handleDisplay();
        if (_ack){
            ++nb;
            updatePoint2Send(nb);
            _ack =false;
        }
        if (_stop) break;
    }
    _programm = PROGRAMMED_MODE;
    return false;
}