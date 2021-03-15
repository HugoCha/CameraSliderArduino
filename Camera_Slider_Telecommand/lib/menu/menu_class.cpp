#include "menu_class.h"

MenuClass::~MenuClass(){}

/*********************************************************************
 * 
 * Radio sending
 * 
*********************************************************************/

void MenuClass::sendInstructions(void){
    if (millis() - _start_time > WAITING_TIME){
        _ack = _radio_telecommand->sendTelecommand();
        _start_time = millis();
    }
}

/***********************************************************************
 * 
 * Buttons
 * 
************************************************************************/

void MenuClass::debounceButton(
    const bool& read_value, 
    bool& last_state, 
    bool& current_state, 
    unsigned long& last_debounce_time)
{
    last_state = read_value;
    if (read_value != last_state) {
        last_debounce_time = millis();
    }

    if ((millis() - last_debounce_time) > DEBOUNCE_DELAY) {
        if (read_value != current_state) {
            current_state = read_value;
            last_state = !current_state;
        }
    }
}


void MenuClass::updateButtons(void){
    _buttons_state.JoyX = analogRead(JOYX);
    _buttons_state.JoyY = analogRead(JOYY);
    _buttons_state.potentiometerX = map(analogRead(POT_X), 0, 865, 0, 200);
    _buttons_state.potentiometerPT = map(analogRead(POT_PT), 0, 1023, 0, 100);
    updateEnterButton();
    updateReturnButton();
    updateFocusButton();
}


void MenuClass::updateEnterButton(void)
{
    static unsigned long last_debounce_time = 0;
    debounceButton(
        digitalRead(ENTER), 
        _last_buttons_state.enter_button,
        _buttons_state.enter_button,
        last_debounce_time);
}


void MenuClass::updateReturnButton(void)
{
    static unsigned long last_debounce_time = 0;
    debounceButton(
        digitalRead(RETURN), 
        _last_buttons_state.return_button,
        _buttons_state.return_button,
        last_debounce_time);
}


void MenuClass::updateFocusButton(void)
{
    static unsigned long last_debounce_time = 0;
    debounceButton(
    digitalRead(FOCUS), 
    _last_buttons_state.focus_button,
    _buttons_state.focus_button,
    last_debounce_time);
}


void MenuClass::handleReturnButton(void)
{
    if (_pause && _buttons_state.return_button && !_last_buttons_state.return_button)
        stop();
    else if (!_pause && _buttons_state.return_button)
        pause();
}

/********************************************************************
 * 
 * Play/Pause/Stop
 * 
********************************************************************/

void MenuClass::play(void){
    _pause = false;
    _start = true;
    _stop = false;
    _radio_telecommand->setState('r');
}


void MenuClass::pause(void){
    _pause =true;
    _start = false;
    _radio_telecommand->setState('p');
}


void MenuClass::stop(void){
    _stop = true;
    _radio_telecommand->setState('s');
}



/*************************************************************************
 * 
 * Display
 * 
**************************************************************************/

void MenuClass::handleDisplay(){
    if (!_ack && _last_screen != 'a' && !_pause){
        display_bad_connection();
        _last_screen = 'a';
    }
    else if (_pause && _last_screen != 'p'){
        display_pause();
        _last_screen = 'p';
    }
    else if (_start && _ack){
        display_motor_info();
        _start = false;
        _last_screen = 's';
    }
    else if (_last_screen != 'a' && _last_screen != 'p'){
        update_display_motor_info();
        _last_screen = 'u';
    }
}


void MenuClass::display_pause()
{
    lcd->clear();
    display_title(false, true, false);
    
    lcd->setCursor(1,2);
    lcd->print("PRESS OK TO RESUME");
    lcd->setCursor(1,3);
    lcd->print("PRESS < TO QUIT");
}

void MenuClass::display_bad_connection(){
    lcd->clear();
    display_title(true, true, true);
    lcd->setCursor(5, 2);
    lcd->print("No connection");
    _start = true;
}


void MenuClass::display_title(const bool& first, const bool& last, const bool& bad_connection){
    if (!first){
        lcd->setCursor(0, 0);
        lcd->print((char) 0x3C);
    }
    if (!last){
        lcd->setCursor(1, 0);
        lcd->print((char) 0x3E);
    }
    
    lcd->setCursor(3, 0);
    lcd->print(title);
    
    lcd->setCursor(19, 0);
    if (bad_connection)
        lcd->write('X');
    else
        lcd->write(byte(6));
}