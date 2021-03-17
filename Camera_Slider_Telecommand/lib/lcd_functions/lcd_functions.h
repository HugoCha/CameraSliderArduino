#ifndef __LCD_FUNCTIONS_H__
#define __LCD_FUNCTIONS_H__

#include <Arduino.h>
#include <stdio.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>

#include <phi_interfaces.h>
#include <phi_prompt.h>

#define MAX_NB_SIZE 5

const int power10[5] = {1, 10, 100, 1000, 10000};

/** 
 * @brief Enter a number with LCD
 * @param lcd Liquide crystarl LCD where to display
 * @param number the number input by user
 * @param nb_size the number of digits to input
 * @param col the column to display number
 * @param row the row to display number
 * @param sign 0 if number is unsigned >0 if is signed
 * @return 0 if no problem -1 else
**/
int enter_number(LiquidCrystal_I2C *lcd, int& number, const uint8_t& nb_size, const uint8_t& col, const uint8_t& row, const uint8_t& sign, const int& min_number=0, const int& max_number=0);

void value_in_limit(phi_prompt_struct* myIntegerInput, int * user_input, const uint8_t& nb_size, const int& i, const int& min_number, const int& max_number, const char* sign_input="x");

int enter3number(LiquidCrystal_I2C *lcd, int number[3], const uint8_t nb_size[3], const uint8_t& row, const uint8_t sign[3], const int min_number[3], const int max_number[3]);

/**
 * @brief Initialize phi_prompt struct for choosing sign 
 * @param sign_input the structure to initialize
 * @param sign_char the buffer for the sign in the structure
 * @param col the column to display the begining of input
 * @param row the row to display the begining of input
 * @param number the last registered number (0 if none)
 * @return void
**/
void initialize_sign_enter(phi_prompt_struct& sign_input, char* sign_char, const uint8_t& col, const uint8_t& row, const int& number=0);


/**
 * @brief Initialize phi_prompt struct for input number
 * @param myIntegerInput An array with the number of digits we want to input (<=5) 
 * @param user_input the array buffer for each digit in the structure must be the same size as myIntegerInput
 * @param nb_size the number of digits we want to input
 * @param col the column to display the begining of input
 * @param row the row to display the begining of input
 * @param sign param =1 if we want to include sign entry (0 by default) 
 * @param number the last registered number (0 if none)
 * @return 0 if no problem, -1 else
**/
int initialize_enter_number(phi_prompt_struct* myIntegerInput, int* user_input, const uint8_t& nb_size, const uint8_t& col, const uint8_t& row, const uint8_t& sign=0, const int& number=0);


/**
 * @brief display a number with max 5 digits + eventually sign
 * @param lcd a pointer to the LCD to display
 * @param number number to display
 * @param nb_size the number of digits we want to display (between 0 and 5)
 * @param col the column to display the begining of input
 * @param row the row to display the begining of input
 * @param sign 1 if we want a sign to be displayed
 * @return void
**/
void display_number(LiquidCrystal_I2C *lcd, const int& number, const uint8_t& nb_size, const uint8_t& col, const uint8_t& row, const uint8_t& sign=0);

/**
 * @brief display a number with number name with 3 digits + eventually sign like X:+458
 * @param lcd a pointer to the LCD to display
 * @param number_name name to display 
 * @param number number to display
 * @param nb_size the number of digits we want to display (between 0 and 5)
 * @param col the column to display the begining of input
 * @param row the row to display the begining of input
 * @param sign 1 if we want a sign to be displayed
 * @return void
**/
void display_3digits_number_with_char(LiquidCrystal_I2C *lcd, const char* number_name, const int& number, const uint8_t& col, const uint8_t& row, const uint8_t& sign=0);

/**
 * @brief function to switch between digits plus sign when input
 * @param myIntegerInput An array of phi_prompt_struct with the number of digits we want to input (<=5) 
 * @param sign_input An array of phi_prompt_struct for register sign
 * @param user_input An array for register digits
 * @param number int where we want to store final value
 * @param nb_size the number of digits we want to input
 * @param add_sign param =1 if we want to include sign entry (0 by default) 
 * @return nb_size+add_sign+1 if user went right (OK or right key), -1 if user went left
**/
int switch_cursor(phi_prompt_struct* myIntegerInput, phi_prompt_struct& sign_input,  int* user_input, int& number, const int& add_sign, const uint8_t& nb_size, const int& min_number, const int& max_number);

/**
 * @brief extract the number from all input of user
 * @param sign_char the buffer where the sign char is stored
 * @param user_input the buffers where all the digits are stored
 * @param nb_size the size of user_input
 * @return the number entered by the user
**/
int extract_number_from_digit(const char *sign_char, const int * user_input, const uint8_t& nb_size);

/**
 * @brief a function to create loading bar with style 
 * @param lcd a pointer to the LCD to display
 * @param current the number of tile we want to fill
 * @param max the max number of tiles we can fill
 * @param col the column to display the begining of bar
 * @param row the row to display the begining of bar
 * @param style the sign we want to display
 * @return void
**/
void loading_bar(LiquidCrystal_I2C *lcd, const uint8_t& current, const uint8_t& max,  const uint8_t& col, const uint8_t& row, const uint8_t& style=255);

/** 
 * @brief a function to create loading bar with style from percentage input
 * @param lcd a pointer to the LCD to display
 * @param percent the percentage fullfilled
 * @param low_col the column to display the begining of bar
 * @param high_col the column to display the end of bar
 * @param row the row to display the begining of bar
 * @param style the sign we want to display
 * @return void
**/
void loading_bar_from_percent(LiquidCrystal_I2C *lcd, const uint8_t &percent, const uint8_t& low_col, const uint8_t& high_col, const uint8_t& row, const uint8_t& style=255);


/** 
 * @brief a function to create loading bar with style from percentage input and display the percentage
 * @param lcd a pointer to the LCD to display
 * @param percent the percentage fullfilled
 * @param row the row to display the begining of bar
 * @param style the sign we want to display
 * @return void
**/
void loading_bar_with_percent_display(LiquidCrystal_I2C *lcd, const uint8_t &percent, const uint8_t& row, const uint8_t& step=1, const uint8_t& style=255);

#endif