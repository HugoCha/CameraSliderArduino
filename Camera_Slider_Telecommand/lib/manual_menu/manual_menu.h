#ifndef __MANUAL_MENU_H__
#define __MANUAL_MENU_H__

#include <LiquidCrystal_I2C.h>

#include "phi_interfaces.h"
#include "phi_prompt.h"
#include "lcd_functions.h"
#include "menu_class.h"

//const char PROGMEM manual_instructions[]="You can control the plateform with the joystick and the speed of the plateform and rail with potentiometers.\n Click OK to continue.";
const char PROGMEM manual_instructions[]="Vous pouvez controler le support avec le joystick, vous pouvez aussi controlez la vitesse du support et du rail avec les potentiometres. Cliquez sur OK pour continuer.";

class ManualMenu : public MenuClass{

public:
    ManualMenu( LiquidCrystal_I2C * screen, uint8_t col, uint8_t row, RadioTelecommand* radio_tel) : MenuClass(screen, col, row, radio_tel, "Comm. Manuelle"){};
    ~ManualMenu();
    bool execute() override;

private:
    int display_instructions(void) override;
    void display_motor_info(void) override;
    void display_motor_info(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Vx, const unsigned short& VangleY, const unsigned short& VangleZ) override;
    void update_display_motor_info(void) override;
    void update_display_motor_info(const int& posX, const int& angleY, const int& angleZ, const unsigned short& Vx, const unsigned short& VangleY, const unsigned short& VangleZ) override;

    bool handleManualMode(void);

    void handleButtons(void);
    void handleEnterButton(void);

    void updateInstructions(void) override;

};

#endif