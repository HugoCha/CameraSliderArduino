#include "main_menu.h"

int number[3] = {0, 0, 0};


void init_top_menu(phi_prompt_struct& myMenu, const uint8_t& lcd_c, const uint8_t& lcd_r){
    // Initialize the top menu
    myMenu.ptr.list=(char**)&top_menu_items; // Assign the list to the pointer
    myMenu.low.i=0; // Default item highlighted on the list
    myMenu.high.i=2; // Last item of the list is size of the list - 1.
    myMenu.width=lcd_c-1; // Length in characters of the longest list item.
    myMenu.step.c_arr[0]=3;//lcd_rows-1; // rows to auto fit entire screen
    myMenu.step.c_arr[1]=1; // one col list
    myMenu.step.c_arr[2]=0; // y for additional feature such as an index
    myMenu.step.c_arr[3]=lcd_c-5; // x for additional feature such as an index
    myMenu.col=0; // Display prompt at column 0
    myMenu.row=1; // Display prompt at row 1
    myMenu.option=45;
}


void print_title(LiquidCrystal_I2C* lcd){
    lcd->clear();  // Refresh menu if a button has been pushed
    //lcd.print("Main Menu");
    lcd->print("Menu Principal");
}


void top_menu(LiquidCrystal_I2C* lcd, RadioTelecommand* radio_tel, const uint8_t& lcd_c, const uint8_t& lcd_r)
{
    //int menu_pointer_1=0; // This stores the menu choice the user made.
    phi_prompt_struct myMenu; // This structure stores the main menu.
    init_top_menu(myMenu, lcd_c, lcd_r);
    OPTION choose_option;
    while(1) // This loops every time a menu item is selected.
    {
        radio_tel->setMenu(RESET);
        while(!radio_tel->sendTelecommand());
        radio_tel->setMenu(WAITING_FOR_USER_CHOICE);
        while(!radio_tel->sendTelecommand());

        print_title(lcd);
        select_list(&myMenu); // Use the select_list to ask the user to select an item of the list, that is a menu item from your menu.
        //menu_pointer_1=myMenu.low.i; // Get the selected item number and store it in the menu pointer.
        switch (myMenu.low.i) // See which menu item is selected and execute that correspond function
        {
        case 0:{
            ManualMenu* manual_menu = new ManualMenu(lcd, lcd_c, lcd_r, radio_tel);
            manual_menu->execute();
            delete manual_menu;
        }break;
            
        case 1:{
            programmed_sub_menu(lcd, lcd_c, lcd_r, radio_tel);}
        break;
        
        case 2:{
            option_sub_menu(lcd, lcd_c, lcd_r, radio_tel);
            }
        break;
        default:
        break;
        }
    }
}