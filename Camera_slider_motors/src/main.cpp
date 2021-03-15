#include "configurations.h"
#include "EEPROM_functions.h"
#include "camera_functions.h"
#include "radio_functions.h"
#include "motor_functions.h"
#include "test_src/test_src_function.h"
#include "battery_functions.h"
#include "core_function.h"
//#include <printf.h>

#define JOY_X A0
#define JOY_Y A1
#define POTX A2
#define POT_PT A3

// AccelStepper* stepper_slider = new AccelStepper(1, STEP1, DIR1);
// AccelStepper* stepper_tilt = new AccelStepper(1, STEP2, DIR2);
// AccelStepper* stepper_pan = new AccelStepper(1, STEP3, DIR3);
// RF24* myradio = new RF24(CE, CSN);
AccelStepper stepper_slider(1, STEP1, DIR1);
AccelStepper stepper_tilt(1, STEP2, DIR2);
AccelStepper stepper_pan(1, STEP3, DIR3);
RF24 myradio(CE, CSN);

MotorInterface Motors(&stepper_slider, &stepper_tilt, &stepper_pan);
Camera camera;
Battery battery;
InstructionHandler instr(&Motors, &camera, &battery);
RadioMotor radio_motor(&myradio, &camera);

unsigned long current_time = 0, start_time=0;
// void setup_test(){
//     //motor_test::launch_test_motor(Motors);
//     test_core::test_core_function(instr);
// }

void init_motor_interface(void);
void show_radio_data(void);
void show_data(void);

void setup() {
    Serial.begin(9600);
    //printf_begin();
    //setup_test();
    Motors.init_motors();
    radio_motor.initRadio();
    // myradio->printDetails();
    //myradio.printDetails();
    Motors.setCurrentPose({500, 0, 0});
}

void loop() {
    current_time = millis();
    if (radio_motor.receive(Motors, battery)){
        instr.execute(radio_motor.getInstructions());

    }
    //if (current_time-start_time>10) show_data();
    Motors.run();   
}

void show_radio_data(void)
{

    unsigned short joyx, joyy;//map(analogRead(JOY_Y), 0,1023, 0,100);
    byte potx, potpt;

    Serial.println();
    Serial.print("Menu selected : ");
    Serial.print(radio_motor.getMenu());
    Serial.println();
    Serial.print("state : ");
    Serial.println(radio_motor.getState());
    Serial.print("Joystick : X : ");
    radio_motor.getJoystick(joyx,joyy);
    Serial.print(joyx);
    Serial.print(" Y : ");
    Serial.print(joyy);
    Serial.print("Potentiometer : X : ");
    radio_motor.getPotentiometer(potx,potpt);
    Serial.print(potx);
    Serial.print(" Y : ");
    Serial.print(potpt);
    Serial.println();
}

void show_data(void)
{
    Serial.println("Motor Info : ");
    Serial.print("Current Pose  : ");
    display_vector3<float,float>(Motors.getCurrentPose());
    Serial.println();
    Serial.print("Current Speed : ");
    display_vector3<float,float>(Motors.getCurrentSpeed());
    Serial.println();
    Serial.print("Max Speed : ");
    display_vector3<float,float>(Motors.getMaxSpeed());
    Serial.println();
    start_time=millis();
}

void init_motor_interface(void){
    Motors.init_motors();
    init_sensors();
}
