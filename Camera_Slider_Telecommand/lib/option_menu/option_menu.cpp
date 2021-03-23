#include "option_menu.h"


void init_option_sub_menu(LiquidCrystal_I2C* lcd, phi_prompt_struct& myMenu, const uint8_t& lcd_c, const uint8_t& lcd_r){

    // Initialize the top menu
    myMenu.ptr.list=(char**)&option_menu_items; // Assign the list to the pointer
    myMenu.low.i=0; // Default item highlighted on the list
    myMenu.high.i=4; // Last item of the list is size of the list - 1.
    myMenu.width=lcd_c-1; // Length in characters of the longest list item.
    myMenu.step.c_arr[0]=3;//lcd_rows-1; // rows to auto fit entire screen
    myMenu.step.c_arr[1]=1; // one col list
    myMenu.step.c_arr[2]=0; // y for additional feature such as an index
    myMenu.step.c_arr[3]=lcd_c-5; // x for additional feature such as an index
    myMenu.col=0; // Display prompt at column 0
    myMenu.row=1; // Display prompt at row 1
    myMenu.option=45;

    // lcd->clear();  // Refresh menu if a button has been pushed
    // lcd->print("Reglages");
}

void option_sub_menu(LiquidCrystal_I2C* lcd, const uint8_t& lcd_c, const uint8_t& lcd_r, RadioTelecommand* radio_tel){
    phi_prompt_struct myMenu; // This structure stores the main menu.
    init_option_sub_menu(lcd, myMenu, lcd_c, lcd_r);
    OPTION current_option;
    while(1){
        lcd->clear();
        lcd->print("Reglages");
        if (select_list(&myMenu) == 1){
            switch (myMenu.low.i)
            {
            case 0:{
                current_option.auto_speed = (bool)yn_dialog("Set Autospeed");
                current_option.option_change |= (1<<4);
            }
                break;
            case 1:{
                int speed = 0;
                lcd->clear();
                lcd->setCursor(0,0);
                lcd->print("Speed Precision");
                enter_number(lcd, speed, 1, 9, 2, 0, 0, 4);
                if (speed == 0) current_option.precision = 1;
                else if (speed == 1) current_option.precision = 2;
                else if (speed == 2) current_option.precision = 4;
                else if (speed == 3) current_option.precision = 8;
                else if (speed == 4) current_option.precision = 16;
                current_option.option_change |= (1<<3);
            }
            break;
            case 2:{
                current_option.auto_mode = (bool)yn_dialog("Set Automode");
                current_option.option_change |= (1<<2);
            }
            break;
            case 3:{
                int delay = 0;
                lcd->clear();
                lcd->setCursor(0,0);
                lcd->print("Delay between pose");
                lcd->setCursor(12, 2);
                lcd->print("s");
                enter_number(lcd, delay, 3, 8, 2, 0);
                current_option.delay = delay;
                current_option.option_change |= (1<<1);
            }
            break;
            case 4:{
                int temp = 0;
                uint8_t nb_size[3] = {3, 3, 3};  
                uint8_t sign[3] = {0, 0, 0};
                int min_number[3] = {0,0,0};
                int max_number[3] = {0,0,0};
                int input_nb_arr[3]= {0, 0, 0};
                display_camera_info(lcd, 2);
                temp = enter3number(lcd, input_nb_arr, nb_size, 2, sign, min_number, max_number);
                if (temp == 1){
                    current_option.h_camera = input_nb_arr[0];
                    current_option.r_lens = input_nb_arr[1];
                    current_option.l_lens = input_nb_arr[2];
                }
                current_option.option_change |= (1<<0);
            }
            default:
                break;
            }
        }
        else{
            radio_tel->setOption(current_option);
            while(!radio_tel->sendOption());
            return;
        }
    }
}


void display_camera_info(LiquidCrystal_I2C* lcd, const uint8_t& row){
    lcd->clear();
    lcd->setCursor(0,0);
    lcd->print("Camera dimension(mm)");
    display_3digits_number_with_char(lcd, "H", 0, 0, row);
    display_3digits_number_with_char(lcd, "R", 0, 7, row);
    display_3digits_number_with_char(lcd, "L", 0, 14,row);
}