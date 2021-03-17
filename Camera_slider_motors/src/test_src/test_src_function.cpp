#include "test_src_function.h"

void write_number_failed(const int& i){
    Serial.print(i);
    Serial.println(" Failed");
    return;
}


namespace motor_test{

// void test_conversion(MotorInterface& Mot){
//     int i=-1;
    
//     if (!assert_equal<int>(Mot.convertmmToStep(40),200)){i=0;}
//     else if (!assert_equal<float>(Mot.convertStepTomm(200), 40)){i=1;}
//     else if (!assert_equal<int>(Mot.convertDegreeToStepPan(10),  16)){i=2;}
//     else if (!assert_equal<int>(Mot.convertStepToDegreePan(16),  10)){i=3;}
//     else if (!assert_equal<int>(Mot.convertDegreeToStepTilt(10), 16)){i=4;}
//     else if (!assert_equal<int>(Mot.convertStepToDegreeTilt(16), 10)){i=5;}
    
//     Serial.print("Conversion test ");
//     Serial.print(i);
//     Serial.println(" Failed");
// }

// void test_limits(MotorInterface& Mot){
//     //Mot.init_motors();
//     Serial.print("Limit test ");
//     bool fail =false;
//     POSE low_lim = {-1, 45, -1};
//     POSE up_lim = {-1, 140, 140};
//     POSE goal1 = {-1, -130, -120};
//     POSE goal2 = {-1, 40, -120};
//     POSE goal3 = {-1, -40, 170};
//     POSE goal4 = {-1, 180, -300};
    
//     if (!assert_equal<bool>(Mot.checkConsistency(goal4), 1)){write_number_failed(0);fail=true;}
//     Mot.setLowLimit(low_lim);
//     if (!assert_equal<bool>(Mot.checkConsistency(goal1), 1)){write_number_failed(1);fail=true;}
//     if (!assert_equal<bool>(Mot.checkConsistency(goal2), 0)){write_number_failed(2);fail=true;}
//     if (!assert_equal<bool>(Mot.checkConsistency(goal3), 0)){write_number_failed(3);fail=true;}
//     if (!assert_equal<bool>(Mot.checkConsistency(goal4), 1)){write_number_failed(4);fail=true;}
    
//     Mot.setUpLimit(up_lim);
//     if (!assert_equal<bool>(Mot.checkConsistency(goal1), 1)){write_number_failed(5);fail=true;}
//     if (!assert_equal<bool>(Mot.checkConsistency(goal2), 0)){write_number_failed(6);fail=true;}
//     if (!assert_equal<bool>(Mot.checkConsistency(goal3), 0)){write_number_failed(7);fail=true;}
//     if (!assert_equal<bool>(Mot.checkConsistency(goal4), 0)){write_number_failed(8);fail=true;}
    
//     if (!fail){
//         Serial.println("No Fail");
//     }
//     return;
// }

// void test_limits(MotorInterface& Mot){
//     POSE lowlim = {10, -30, -40};
//     POSE uplim = {100, 40, 180};
//     Mot.setLimits(lowlim, uplim);

//     Mot.play();
//     int i=30;
//     float j=30;
    // while (i>0)
    // {
    //     Mot.setCurrentPose({i,(-35+j), (-45+j)});
    //     display_vector3<float, float>(Mot.getCurrentPose());
    //     Serial.println();
    //     Serial.println(Mot.checkSliderLimit());
    //     Serial.println(Mot.checkTiltLimit());
    //     Serial.println(Mot.checkPanLimit());
    //     Serial.println();
    //     --i;j--;
    // }
    // i=0;
    // j=0;
    // while (i<30)
    // {
    //     Mot.setCurrentPose({i,(325+j), (315+j)});
    //     display_vector3<float, float>(Mot.getCurrentPose());
    //     Serial.println();
    //     Serial.println(Mot.checkSliderLimit());
    //     Serial.println(Mot.checkTiltLimit());
    //     Serial.println(Mot.checkPanLimit());
    //     Serial.println();
    //     ++i;++j;
    // }
    // i=0;j=0;
    // while (i<30)
    // {
    //     Mot.setCurrentPose({i+90,(35+j), (179+j)});
    //     display_vector3<float, float>(Mot.getCurrentPose());
    //     Serial.println();
    //     Serial.println(Mot.checkSliderLimit());
    //     Serial.println(Mot.checkTiltLimit());
    //     Serial.println(Mot.checkPanLimit());
    //     Serial.println();
    //     ++i;++j;
    // }
    // i=30;
    // j=0;
    // while (i>0)
    // {
    //     Mot.setCurrentPose({i,(340+j), (-45+j)});
    //     display_vector3<float, float>(Mot.getCurrentPose());
    //     Serial.println();
    //     Serial.println(Mot.checkSliderLimit());
    //     Serial.println(Mot.checkTiltLimit());
    //     Serial.println(Mot.checkPanLimit());
    //     Serial.println();
    //     --i;j--;
    // }
    
//}

void test_sensors(MotorInterface& Mot){
    Serial.println("Sensor Test");
    unsigned long start_time=0, current_time=0;
    while (current_time-start_time < 5000){
        Serial.print("Check Sensor : ");
        display_vector3<bool, bool>(check_sensor());
        Serial.println();
        current_time = millis();
    }
    start_time = current_time;
    // while (current_time-start_time < 5000){
    //     Serial.println("Assure Change : ");
    //     SENSOR_STATE sense = check_sensor();
    //     Serial.print("Slider : ");
    //     Serial.print(Mot.isNewSensorStateSlider(sense.slider));
    //     Serial.print(" Pan : ");
    //     Serial.print(Mot.isNewSensorStatePan(sense.pan));
    //     Serial.print(" Tilt : ");
    //     Serial.println(Mot.isNewSensorStateTilt(sense.tilt));
    //     display_vector3<bool, bool>(Mot.getSensorState());
    //     Serial.println();
    //     current_time = millis();
    // }
    start_time = current_time;
    while (current_time-start_time < 15000){
        Serial.print("Current Pose reset : ");
        POSE testpose = Mot.getCurrentPose();
        testpose.slider++;testpose.pan+=1.0; testpose.tilt+=2.0;
        Mot.setCurrentPose({testpose.slider,testpose.tilt,testpose.pan});
        Mot.sensorDetect();
        display_vector3<float, float>(testpose);
        current_time = millis();
        Serial.println();
    }
    Serial.println();
}


void test_stop_to_limits(MotorInterface& Mot){
//     bool fail=false;
//     Serial.println("Test Stop to limit : ");
//     Mot.setLowLimit({10, 30, 30});
//     Mot.setUpLimit({100, 60, 70.5});
//     Serial.print("Low Limits : ");
//     display_vector3<float, float>(Mot.getLowLimits());
//     Serial.println();
//     Serial.print("Up Limits : ");
//     display_vector3<float, float>(Mot.getUpLimits());
//     Serial.println();

//     Mot.setCurrentPose({0, 20, 10});
//     if ((!assert_equal<bool>(Mot.needToStopSlider(), 1))
//     || (!assert_equal<bool>(Mot.needToStopTilt(), 1))
//     || (!assert_equal<bool>(Mot.needToStopPan(), 1))){write_number_failed(0);fail=true;} 

//     Mot.setCurrentPose({10, 30, 30});
//     if ((!assert_equal<bool>(Mot.needToStopSlider(), 0))
//     || (!assert_equal<bool>(Mot.needToStopTilt(), 0))
//     || (!assert_equal<bool>(Mot.needToStopPan(), 0))){write_number_failed(1);fail=true;}

//     Mot.setCurrentPose({50, 35, 40});
//     if ((!assert_equal<bool>(Mot.needToStopSlider(), 0))
//     || (!assert_equal<bool>(Mot.needToStopTilt(), 0))
//     || (!assert_equal<bool>(Mot.needToStopPan(), 0))){write_number_failed(2);fail=true;} 

//     Mot.setCurrentPose({100, 60, 70.5});
//     if ((!assert_equal<bool>(Mot.needToStopSlider(), 0))
//     || (!assert_equal<bool>(Mot.needToStopTilt(), 0))
//     || (!assert_equal<bool>(Mot.needToStopPan(), 0))){write_number_failed(3);fail=true;}

//     Mot.setCurrentPose({110, 65, 80});
//     if ((!assert_equal<bool>(Mot.needToStopSlider(), 1))
//     || (!assert_equal<bool>(Mot.needToStopTilt(), 1))
//     || (!assert_equal<bool>(Mot.needToStopPan(), 1))){write_number_failed(4);fail=true;} 


//     if (!fail){
//         Serial.println("No fail!");
//     }
}

void test_set_pose_goal(MotorInterface& Mot){
    Serial.println("Test set Pose Goal : ");
    bool fail = false;
    Mot.setLowLimit({10, 30, 20});
    
    if (!assert_equal<bool>(Mot.setPoseGoal({-10, 30, 40}), 0)){write_number_failed(0);fail=true;}
    if (!assert_equal<bool>(Mot.setPoseGoal({20, 30, 40}), 1)){write_number_failed(1);fail=true;}
    if (!assert_equal<bool>(Mot.setPoseGoal({20, -20, 40}), 0)){write_number_failed(2);fail=true;}
    if (!assert_equal<bool>(Mot.setPoseGoal({20, 30, 10}), 0)){write_number_failed(3);fail=true;}
    if (!assert_equal<bool>(Mot.setPoseGoal({200, 300, 400}), 1)){write_number_failed(4);fail=true;}
    
    Mot.setUpLimit({100, 60, 70.5});

        if (!assert_equal<bool>(Mot.setPoseGoal({120, 30, 40}), 0)){write_number_failed(5);fail=true;}
    if (!assert_equal<bool>(Mot.setPoseGoal({20, 70, 40}), 0)){write_number_failed(6);fail=true;}
    if (!assert_equal<bool>(Mot.setPoseGoal({20, -20, 40}), 0)){write_number_failed(7);fail=true;}
    if (!assert_equal<bool>(Mot.setPoseGoal({20, 30, 110}), 0)){write_number_failed(8);fail=true;}
    if (!assert_equal<bool>(Mot.setPoseGoal({30, -30, -40}), 1)){write_number_failed(9);fail=true;}

    if (!fail){
        Serial.println("No Fail !");
    }
}

void test_update_direction(MotorInterface& Mot){
    Serial.println("Test Update Direction : ");
    Mot.setLowLimit({10, 30, 10});
    Mot.setUpLimit({100, 60, 70.5});
    Mot.setCurrentPose({20, 40, 20});
    
    Mot.setPoseGoal({15, 31, 11});
    display_vector3<bool, bool>(Mot.getDirection());
    Serial.println();

    Mot.setPoseGoal({30, 45, 45});
    display_vector3<bool, bool>(Mot.getDirection());
    Serial.println();

    Mot.setPoseGoal({10, 30, 30});
    display_vector3<bool, bool>(Mot.getDirection());
    Serial.println();
}

void test_set_home_pose(MotorInterface& Mot){
    Serial.println("Test Set Home Pose : ");
    Mot.setLowLimit({10, 30, 20});
    Mot.setUpLimit({100, 60, 70.5});
    Mot.setCurrentPose({20, 40, 20});
    Mot.setPoseGoal({15, 31, 11});

    // Mot.setCurrentSliderPoseHomePose();
    // Mot.setCurrentTiltPoseHomePose();
    // Mot.setCurrentPanPoseHomePose();
    
    // display_vector3<float, float>(Mot.getCurrentPose());
    // Serial.println();

    // Mot.setPoseGoal({100, 41, 31});
    // Mot.setCurrentSliderPoseHomePose();
    // Mot.setCurrentTiltPoseHomePose();
    // Mot.setCurrentPanPoseHomePose();
    
    // display_vector3<float, float>(Mot.getCurrentPose());
    // Serial.println();

}


void test_conversion(MotorInterface& Mot){
    Serial.println("Test conversion :");
    // Serial.print("mm to step : ");
    // Serial.println(Mot.convertmmToStep(10));
    // Serial.print("step to mm : ");
    // Serial.println(Mot.convertStepTomm(10));
    // Serial.print("degree to step pan : ");
    // Serial.println(Mot.convertDegreeToStepPan(360));
    // Serial.print("step to degree pan : ");
    // Serial.println(Mot.convertStepToDegreePan(10));
    // Serial.print("degree to step tilt : ");
    // Serial.println(Mot.convertDegreeToStepTilt(10));
    // Serial.print("step to degree tilt : ");
    // Serial.println(Mot.convertStepToDegreeTilt(10));
}

void test_interpolate_speed(MotorInterface& Mot){
    // Serial.println("Test Interpolate Speed : ");
    // Mot.setCurrentPose({0, 55, 70});
    // Mot.setPoseGoal({100, 50, 150});
    // Mot.setAutoSpeed(true);
    // Mot.convertPotToMaxSpeed(100, 90);

    // Serial.println("Computed Speed : ");
    // display_vector3<float, float>(Mot.getCurrentSpeed());
    // Serial.println();
    // display_vector3<float,float>(Mot.getMaxSpeed());
    // Serial.println();
}

void launch_test_motor(MotorInterface& Mot){
    // Mot.init_motors();
    // init_sensors();
    // Serial.println("---------------------------------------------");
    // Serial.println("Motor test : ");
    // //test_conversion(Mot);
    // //test_limits(Mot);
    // //test_sensors(Mot);
    // //test_stop_to_limits(Mot);
    // //test_set_pose_goal(Mot);
    // //test_update_direction(Mot);
    // //test_set_home_pose(Mot);
    // //test_conversion(Mot);
    // //test_interpolate_speed(Mot);
    // Serial.println("---------------------------------------------");
}

}


namespace test_camera{

void test_compute_limits(Camera& cam){
    Serial.println("Test compute limits : ");
    float lim[2];
    cam.setCameraHeight(0);
    cam.setLensLength(0);
    cam.setRadiusLength(0);
    cam.computeTiltLimits(lim);
    if (!assert_equal<int>(lim[0], 3.15)){Serial.println("Low Limit set while should not be");}
    if (!assert_equal<int>(lim[1], 3.15)){Serial.println("High Limit set while should not be");}


    cam.setCameraHeight(110);
    cam.setLensLength(70);
    cam.setRadiusLength(30);
    cam.setCameraLength(50);
    cam.computeTiltLimits(lim, 0.1);
    Serial.print("Low Limit Pan : ");
    Serial.println(lim[0]*180/PI);
    Serial.print("Up Limit Pan : ");
    Serial.println(lim[1]*180/PI);
    //assert_equal<int>(lim[0], -1);
    //assert_equal<int>(lim[1], -1);
}


void test_shoot(Camera& cam){

    cam.activate();
    cam.shoot();
}
void test_focus(Camera& cam){
    cam.activate();
    cam.focus();
}
}


namespace test_core{

// void testAddPose(InstructionHandler& instr){
//     Serial.println("Initial Array : ");
//     for (int i=0; i<TARGET_ARRAY_SIZE;++i){
//         Serial.print(i);Serial.print(" : ");
//         display_vector3<float,float>(instr._target_pose[i]);
//         Serial.println();
//     }
//     Serial.println("Test add pose int 3 : ");
//     int pose2add[3] = {999, 999, 999};
//     instr.addPose(pose2add);
//     display_vector3<float,float>(instr._target_pose[0]);
//     Serial.println();
//     Serial.println("Add pose to Target Array : ");
//     instr.addPose({12, 34, -394});
//     instr.addPose({34, 384, 849});
//     instr.addPose({34, 38, 849});
//     instr.addPose({3, 384, 849});
//     instr.addPose({4, 384, 849});
//     instr.addPose({34, 34, 849});
//     for (int i=0; i<TARGET_ARRAY_SIZE;++i){
//         Serial.print(i);Serial.print(" : ");
//         display_vector3<float,float>(instr._target_pose[i]);
//         Serial.println();
//     }
//     Serial.print("Number of pose : "); 
//     Serial.println(instr._number_of_pose);

//     Serial.println("Clear Last Pose : ");
//     instr.clearLastPose();
//     for (int i=0; i<TARGET_ARRAY_SIZE;++i){
//         Serial.print(i);Serial.print(" : ");
//         display_vector3<float,float>(instr._target_pose[i]);
//         Serial.println();
//     }
//     Serial.print("Number of pose : "); 
//     Serial.println(instr._number_of_pose);

//     Serial.println("Clear All Pose : ");
//     instr.clearAllPose();
//     for (int i=0; i<TARGET_ARRAY_SIZE;++i){
//         Serial.print(i);Serial.print(" : ");
//         display_vector3<float,float>(instr._target_pose[i]);
//         Serial.println();
//     }
//     Serial.print("Number of pose : "); 
//     Serial.println(instr._number_of_pose);
// }


// void test_teleop_register_mode(InstructionHandler& instr){
//     instr._instructions.menu.pose_number = 0;
//     instr._instructions.menu.PointA[0] = 999;
//     instr._instructions.menu.PointA[1] = 999;
//     instr._instructions.menu.PointA[2] = 999;

//     instr.registerTeleopMode();

//     Serial.println("Register teleop test : ");
//     Serial.print("Target Array 0 : ");
//     display_vector3<float,float>(instr._target_pose[0]);
//     Serial.println();

//     instr.registerTeleopMode();

//     Serial.println("Register teleop test 2 time without change : ");
//     Serial.print("Target Array 0 : ");
//     display_vector3<float,float>(instr._target_pose[0]);
//     Serial.println();
//     Serial.print("Number of pose : ");
//     Serial.println(instr._number_of_pose);
    

//     Serial.println("Register teleop test add new pose : ");
//     instr._instructions.menu.pose_number = 1;
//     instr.registerTeleopMode();
//     Serial.print("Target Array 0 : ");
//     display_vector3<float,float>(instr._target_pose[0]);
//     Serial.println();
//     Serial.print("Target Array 1 : ");
//     display_vector3<float,float>(instr._target_pose[1]);
//     Serial.println();

//     Serial.println("Register teleop test cancel pose : ");
//     instr._instructions.menu.pose_number = 0;
//     instr.registerTeleopMode();
//     Serial.print("Target Array 0 : ");
//     display_vector3<float,float>(instr._target_pose[0]);
//     Serial.println();
//     Serial.print("Target Array 1 : ");
//     display_vector3<float,float>(instr._target_pose[1]);
//     Serial.println();

//     Serial.println("Register teleop test back with pose 0 : ");
//     instr._instructions.menu.pose_number = -1;
//     instr.registerTeleopMode();
//     Serial.print("Target Array 0 : ");
//     display_vector3<float,float>(instr._target_pose[0]);
//     Serial.println();
//     Serial.print("Target Array 1 : ");
//     display_vector3<float,float>(instr._target_pose[1]);
//     Serial.println();


//     Serial.println("Register teleop test pose 0 : ");
//     instr._instructions.menu.pose_number = 0;
//     instr.registerTeleopMode();
//     Serial.print("Target Array 0 : ");
//     display_vector3<float,float>(instr._target_pose[0]);
//     Serial.println();
//     Serial.print("Target Array 1 : ");
//     display_vector3<float,float>(instr._target_pose[1]);
//     Serial.println();
// }


// void test_programmed_mode(InstructionHandler& instr){
//     Serial.println("Test Programmed Mode : ");
//     instr._motor.setCurrentPose({50, 0, 0});
//     instr._instructions.PotentiometerX = 100;
//     instr._instructions.PotentiometerPT = 100;
//     instr.play();

//     instr._motor.setAutoSpeed(true);
//     instr._motor.setDelay(500);
    
//     Serial.println("Add Pose... ");
//     instr.addPose({70, 10, 10});
//     instr.addPose({70, -10, 10});
//     instr.addPose({50, 10, -30});
//     instr.addPose({100, 200, 200});
//     instr.addPose({70, 10, 10});

//     Serial.println("Programmed Mode begin : ");
//     unsigned long start_time = 0, current_time = millis();
//     while (!instr.programmedMode()){
//         if (current_time-start_time>2){
//             Serial.print("Current Pose  : ");
//             display_vector3<float, float>(instr._motor.getCurrentPose());
//             Serial.println();

//             Serial.print("Current Speed : ");
//             display_vector3<float, float>(instr._motor.getCurrentSpeed());
//             Serial.println();
//             start_time = millis();
//         }
//         current_time = millis();
//         instr._motor.run();
//     }
//     Serial.println("Programmed mode over.");
// }

// void test_auto_mode(InstructionHandler& instr){
//     Serial.println("Test Auto Mode : ");
//     instr._motor.setCurrentPose({50, 0, 0});
//     instr._instructions.PotentiometerX = 100;
//     instr._instructions.PotentiometerPT = 100;
//     instr.play();

//     instr._motor.setAutoSpeed(true);
//     instr._motor.setDelay(500);
    
//     Serial.println("Add Pose... ");
//     instr.addPose({70, 10, 10});
//     instr.addPose({70, -10, 10});
//     instr.addPose({50, 10, -30});

//     Serial.println("Programmed Mode begin : ");
//     unsigned long start_time = 0, current_time = millis(), running_time = millis();
//     while (running_time-start_time < 10000){    
//         instr.autoMode();
//         if (current_time-start_time>2){
//             Serial.print("Current Pose  : ");
//             display_vector3<float, float>(instr._motor.getCurrentPose());
//             Serial.println();

//             Serial.print("Current Speed : ");
//             display_vector3<float, float>(instr._motor.getCurrentSpeed());
//             Serial.println();
//             start_time = millis();
//         }
//         running_time = current_time = millis();
//         instr._motor.run();
//     }
//     Serial.println("Auto mode over.");
// }


// void test_angle_from_pose(InstructionHandler& instr){
//     Serial.println("Test compute Pan Angle from Pose  : ");
//     Serial.println(instr.computePanAngleFromPose(500, 500, 0));
//     Serial.println(instr.computePanAngleFromPose(500, 500, 500));
//     Serial.println(instr.computePanAngleFromPose(500, 500, 1000));

//     Serial.println(instr.computePanAngleFromPose(-100, -101, 100));
//     Serial.println(instr.computePanAngleFromPose(-100, -500, 600));

//     Serial.println("Test compute Tilt Angle from Pose  : ");
//     Serial.println(instr.computeTiltAngleFromPose(10, -20));
//     Serial.println(instr.computeTiltAngleFromPose(120, 200));
//     Serial.println(instr.computeTiltAngleFromPose(220, 200));
//     Serial.println(instr.computeTiltAngleFromPose(320, 200));

// }


// void test_register_tracking(InstructionHandler& instr){
//     Serial.println("Test Register Tracking mode : ");
    
//     Serial.println("Register object pose : ");
//     instr._instructions.menu.PointA[0] = 100;
//     instr._instructions.menu.PointA[1] = 100;
//     instr._instructions.menu.PointA[2] = 220;
//     instr.registerTrackingMode();
//     Serial.println("Object Pose : ");
//     for (int i=0; i<2;++i)
//         Serial.println(instr._object_pose[i]);
    
//     Serial.println();
//     Serial.println("Register slider start pose");
//     instr._instructions.menu.PointA[0] = 100;
//     instr._instructions.menu.PointA[1] = 999;
//     instr._instructions.menu.PointA[2] = 999;
//     instr.registerTrackingMode();


//     Serial.println("Register slider end pose");
//     instr._instructions.menu.PointA[0] = 400;
//     instr._instructions.menu.PointA[1] = 999;
//     instr._instructions.menu.PointA[2] = 999;
//     instr.registerTrackingMode();

//     Serial.println("target pose array : ");
//     for (int i=0; i<TARGET_ARRAY_SIZE;++i){
//         display_vector3<float,float>(instr._target_pose[i]);
//         Serial.println();
//     }
//     Serial.println("Register Mode over");

// }


// void test_register_hyperlapse(InstructionHandler& instr){
//     Serial.println("Test Hyerlapse : ");

//     instr._number_of_photo = 13;
//     instr._motor.setCurrentPose({90, 100, 220});
//     instr._instructions.PotentiometerX = 100;
//     instr._instructions.PotentiometerPT = 100;
//     instr.play();

//     instr._motor.setAutoSpeed(true);
//     instr._motor.setDelay(10);

//     Serial.println("Register object pose : ");
//     instr._instructions.menu.PointA[0] = 100;
//     instr._instructions.menu.PointA[1] = 100;
//     instr._instructions.menu.PointA[2] = 220;
//     instr.registerHyperlapseMode();

//     instr._instructions.menu.PointA[0] = 500;
//     instr._instructions.menu.PointA[1] = 300;
//     instr._instructions.menu.PointA[2] = 220;
//     instr.registerHyperlapseMode();

//     // for (int i=0; i<TARGET_ARRAY_SIZE;++i){
//     //     display_vector3<float, float>(instr._target_pose[i]);
//     //     Serial.println();
//     // }

//     instr.hyperlapseMode();
//     for (int i=0; i<TARGET_ARRAY_SIZE;++i){
//         display_vector3<float, float>(instr._target_pose[i]);
//         Serial.println();
//     }
//     unsigned long current_time=millis(), start_time=millis();
//     while (!instr.hyperlapseMode()){
        
//         if (current_time-start_time>2){
//             Serial.print("Current Pose : ");
//             display_vector3<float,float>(instr._motor.getCurrentPose());
//             Serial.println();
//             // for (int i=0; i<TARGET_ARRAY_SIZE;++i){
//             //     display_vector3<float, float>(instr._target_pose[i]);
//             //     Serial.println();
//             // }
//             start_time = millis();
//         }
//         current_time = millis();
//         instr._motor.run();
//     }
//     Serial.println("Hyperlapse Mode over.");
//}

void test_core_function(InstructionHandler& instr){
    Serial.println("Test core : ");
    //testAddPose(instr);
    //test_teleop_register_mode(instr);
    //test_programmed_mode(instr);
    //test_auto_mode(instr);
    //test_angle_from_pose(instr);
    //test_register_tracking(instr);
    //test_register_hyperlapse(instr);
}
}