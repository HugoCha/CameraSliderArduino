#include <lcd_functions.h>


int enter_number(LiquidCrystal_I2C *lcd, int& number, const uint8_t& nb_size, const uint8_t& col, const uint8_t& row, const uint8_t& sign, const int& min_number,const int& max_number){
    int user_input[nb_size]; // This is the storage for the integer
    int add_sign = (sign>0)? 1:0;
    phi_prompt_struct myIntegerInput[nb_size];
    char sign_char[]= "+";
    int action_return=0;

    if (number < 0)
        strcpy(sign_char, "-");
    
    phi_prompt_struct sign_input;
    initialize_sign_enter(sign_input, sign_char,  col, row, number);

    if (initialize_enter_number(myIntegerInput, user_input, nb_size, col, row, add_sign, number) < 0){return -1;}
    display_number(lcd, number, nb_size, col, row, add_sign);
    action_return = switch_cursor(myIntegerInput, sign_input, user_input, number, add_sign, nb_size, min_number, max_number);
    
    return action_return;
}

int enter3number(LiquidCrystal_I2C *lcd, int number[3], const uint8_t nb_size[3], const uint8_t& row, const uint8_t sign[3], const int min_number[3], const int max_number[3]){
    int action, i=0;
    for (int j=0; j<3; ++j)
        display_number(lcd, number[j], nb_size[j], 7*j+2, row, sign[j]);

    while (!(action>0 && i==3)){
        action = enter_number(lcd, number[i], nb_size[i], 7*i+2, row, sign[i], min_number[i], max_number[i]);
        if (action>=0){++i;}
        else if (action == -1){
            if (i==0){return -1;}
            else {--i;}
        }
        else{return -2;}
    }
    return (i>0)?1:i;
}

void initialize_sign_enter(phi_prompt_struct& sign_input, char* sign_char, const uint8_t& col, const uint8_t& row, const int& number){
    sign_input.ptr.msg = sign_char;
    sign_input.col     = col;
    sign_input.low.c   = '-';
    sign_input.high.c  = '+';
    sign_input.row     = row;
    sign_input.width   = 1;
    sign_input.option  = 0;
}


int initialize_enter_number(phi_prompt_struct* myIntegerInput, int* user_input, const uint8_t& nb_size, const uint8_t& col, const uint8_t& row, const uint8_t& sign, const int& number){
    if (nb_size > MAX_NB_SIZE){return -1;}
    
    if (col + nb_size > 20){return -1;}
    
    for (uint8_t i=0; i<nb_size; ++i){
        user_input[i] = (abs(number)/power10[nb_size-1-i]) % 10;

        myIntegerInput[i].ptr.i_buffer=&user_input[i]; // Pass the address of the buffer. After library function call, user input will be stored in this variable.
        myIntegerInput[i].low.i  = 0; // Lower limit of the number is 0
        myIntegerInput[i].high.i = 9; // Upper limit of the number is 20
        myIntegerInput[i].step.i = 1; // Step size. User choice increments 2 every time you push up and decrements 2 when you press down.
        myIntegerInput[i].col    = col+i+sign; // Display number at column 7
        myIntegerInput[i].row    = row; // Display number at row 1
        myIntegerInput[i].width  = 1; // The number occupies 2 characters space.
        myIntegerInput[i].option = 0; // Option 0, space pad right, option 1, zero pad left, option 2, space pad left.
    }

    return 0;
}

void display_number(LiquidCrystal_I2C *lcd, const int& number, const uint8_t& nb_size, const uint8_t& col, const uint8_t& row, const uint8_t& sign){
    if (sign){
        lcd->setCursor(col, row);
        if (number>=0){lcd->print("+");}
        else {lcd->print("-");}
    }
    
    for (uint8_t i=0; i<nb_size;++i){
        lcd->setCursor(col+sign+i, row);
        lcd->print(abs(number)/power10[nb_size-i-1]%10);
    }
}


void display_3digits_number_with_char(LiquidCrystal_I2C *lcd, const char* number_name, const int& number, const uint8_t& col, const uint8_t& row, const uint8_t& sign){
    lcd->setCursor(col, row);
    lcd->print(number_name);
    lcd->setCursor(col+strlen(number_name), row);
    lcd->print(":");
    display_number(lcd, number, 3, col+strlen(number_name)+1, row, sign);
}


int switch_cursor(phi_prompt_struct* myIntegerInput, phi_prompt_struct& sign_input,  int* user_input, int& number, const int& add_sign, const uint8_t& nb_size, const int& min_number, const int& max_number){
    int input_int = 0;
    int i = 0;
    while (i>=0 && i<nb_size+add_sign){
        if (add_sign && i==0)
            input_int = input_panel(&sign_input);
        else{
            value_in_limit(myIntegerInput, user_input, nb_size, i-add_sign, min_number, max_number);
            input_int = input_integer(&myIntegerInput[i-add_sign]);
        }
        if (input_int == 1 || input_int == -4){
            ++i;
        }
        else if (input_int == -3 || input_int == -1){
            --i;    
        }
        else{
            i=-2;
            break;
        }
    }
    number = extract_number_from_digit(sign_input.ptr.msg, user_input, nb_size);
    return i;
}


bool value_in_limit(phi_prompt_struct* myIntegerInput, int * user_input, const uint8_t& nb_size, const int& i, const int& min_number, const int& max_number){
    if (max_number>=0 || min_number>=0){
        int low_limit = min_number/power10[nb_size-1-i] % 10;
        int up_limit = max_number/power10[nb_size-1-i] % 10;
        bool up_limit_reach = ((user_input[i-1] == myIntegerInput[i-1].high.i && user_input[0]==myIntegerInput[0].high.i) && max_number>=0);
        bool low_limit_reach = ((user_input[i-1] == myIntegerInput[i-1].low.i && user_input[0]==myIntegerInput[0].low.i) && min_number>=0);
        if (i==0){
            myIntegerInput[i].low.i = low_limit;
            myIntegerInput[i].high.i = up_limit;
        }
        else if (up_limit_reach || low_limit_reach){
            myIntegerInput[i].high.i = (up_limit_reach)?up_limit:9;
            myIntegerInput[i].low.i = (low_limit_reach)?low_limit:0;
            user_input[i] = (user_input[i]>=up_limit && up_limit_reach)?up_limit:user_input[i];
            user_input[i] = (user_input[i]<=low_limit && low_limit_reach)?low_limit:user_input[i];
            return true;
        }
        else{
            myIntegerInput[i].high.i = 9;
        }
    }
    return false;
}


int extract_number_from_digit(const char* sign_char, const int * user_input, const uint8_t& nb_size){
    int input_number=0;
    for (uint8_t i=0; i<nb_size; ++i)
        input_number += user_input[i] * power10[nb_size-1-i];
    return (!strcmp(sign_char, "-"))?-input_number:input_number;
}


void loading_bar(LiquidCrystal_I2C *lcd, const uint8_t& current, const uint8_t& max,  const uint8_t& col, const uint8_t& row, const uint8_t& style){
    uint8_t nb_2_display=0; 
    if (max+col<=20 && row<=4){
        if (max < current){
            nb_2_display = max;
        }
        else{
            nb_2_display = current;
        }
        lcd->setCursor(col, row);

        for ( uint8_t i = 0; i <= nb_2_display; i++)
        {
            lcd->print((char) style);
        }   
        
    }
}

void loading_bar_from_percent(LiquidCrystal_I2C *lcd, const uint8_t &percent, const uint8_t& low_col, const uint8_t& high_col, const uint8_t& row, const uint8_t& style){
    uint8_t current = (float)percent/100 * (high_col - low_col);
    loading_bar(lcd, current, high_col-low_col, low_col, row, style);
}


void loading_bar_with_percent_display(LiquidCrystal_I2C *lcd, const uint8_t &percent, const uint8_t& row, const uint8_t& step, const uint8_t& style){
    loading_bar_from_percent(lcd, percent,1, 15, row, style);
    
    if (percent%step == 0){
        if (percent<100) 
            lcd->setCursor(17, row);
        else
            lcd->setCursor(16, row);
        lcd->print(percent);
        lcd->setCursor(19, row);
        lcd->print((char) 0x25);
    }
}