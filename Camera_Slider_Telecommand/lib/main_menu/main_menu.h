#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>


#include "phi_interfaces.h"
#include "phi_prompt.h"

#include "manual_menu.h"
#include "programmed_menu.h"
#include "option_menu.h"
#include "lcd_functions.h"
#include "radio_function.h"

const char PROGMEM top_menu_item00[]="Commande Manuelle";
const char PROGMEM top_menu_item01[]="Parcours Programme";
const char PROGMEM top_menu_item02[]="Reglages";
const char PROGMEM * const top_menu_items[] = {top_menu_item00, top_menu_item01, top_menu_item02};

void init_top_menu(phi_prompt_struct& menu, const uint8_t& lcd_c, const uint8_t& lcd_r);
void print_title(LiquidCrystal_I2C* lcd);
void top_menu(LiquidCrystal_I2C* lcd, RadioTelecommand* radio_tel, const uint8_t& lcd_c=20, const uint8_t& lcd_r=4);


#endif