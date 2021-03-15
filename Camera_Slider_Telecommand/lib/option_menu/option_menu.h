#ifndef __OPTION_MENU_H__
#define __OPTION_MENU_H__

#include "menu_class.h"
#include "radio_function.h"
#include "lcd_functions.h"


const char PROGMEM option_menu_item00[]="Autospeed";
const char PROGMEM option_menu_item01[]="Speed";
const char PROGMEM option_menu_item02[]="Automode";
const char PROGMEM option_menu_item03[]="Delay";
const char PROGMEM option_menu_item04[]="Camera Dimension";
const char PROGMEM * const option_menu_items[] = {option_menu_item00, option_menu_item01, option_menu_item02, option_menu_item03, option_menu_item04};

void init_option_sub_menu(LiquidCrystal_I2C* lcd, phi_prompt_struct& myMenu, const uint8_t& lcd_c, const uint8_t& lcd_r);
void option_sub_menu(LiquidCrystal_I2C* lcd, const uint8_t& lcd_c, const uint8_t& lcd_r, RadioTelecommand* radio_tel);
void display_camera_info(LiquidCrystal_I2C* lcd, const uint8_t& row);

#endif