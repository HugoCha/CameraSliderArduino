#include <Arduino.h>
#include <stdio.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>

#include "configurations.h"

#include <phi_interfaces.h>
#include <phi_prompt.h>
#include "printf.h"

#include "main_menu.h"

char mapping1[]={'0','1', '2', '3', NO_KEY, '4', '5', '6', '7'}; // This is a rotary encoder so it returns U for up and D for down on the dial.
byte Joy_pins[] = {JOYX, JOYY};
int value[] = {0, 512, 1024, 0, 512, 1024};
int th = 100;
phi_joysticks joystick_but(mapping1, Joy_pins, value, th);


char mapping2[] = {'E', 'R'};
byte SW_pins[] = {SW, RETURN};
byte logic[] = {0, 1};
phi_button_groups SW_joy(mapping2, SW_pins, logic, 2);
multiple_button_input* joy[] = {&joystick_but, &SW_joy, 0};
char up_keys[]={'5','6','7',0}; ///< All keys that act as the up key are listed here.
char down_keys[]={'0', '1', '2',0}; ///< All keys that act as the down key are listed here.
char left_keys[]={'3',0}; ///< All keys that act as the left key are listed here.
char right_keys[]={'4', 0}; ///< All keys that act as the right key are listed here.
char enter_keys[]={'E', 0};
char escape_keys[]={'R', 0};
char * function_keys[]={up_keys,down_keys,left_keys,right_keys, enter_keys, escape_keys};

RF24* myradio = new RF24(CE, CSN);
LiquidCrystal_I2C* lcd = new LiquidCrystal_I2C(0x27, lcd_columns, lcd_rows);
RadioTelecommand* radio_tel = new RadioTelecommand(myradio);


void setup() {
    lcd->init();
    lcd->backlight();
    // Print a message to the LCD.
    Serial.begin(9600);
    Serial.println("LCD Menu");
    //printf_begin();
    radio_tel->initRadio();
    //myradio->printDetails();
    init_phi_prompt(lcd, joy, function_keys, lcd_columns, lcd_rows, '~');
}

void loop() {
    //lcd->print("No connection");
    top_menu(lcd, radio_tel, lcd_columns, lcd_rows);
}