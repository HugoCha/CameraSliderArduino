#ifndef __TEST_SRC_FUNCTION__
#define __TEST_SRC_FUNCTION__

#include "motor_functions.h"
#include "camera_functions.h"
#include "core_function.h"

template<typename T>
bool assert_equal(T x, T y){
    return (x == y);
}

template <typename T, typename T1>
void display_vector3(const struct Vector3<T, T1>& V1){
    Serial.print("Slider : ");
    Serial.print(V1.slider);
    Serial.print(" Tilt : ");
    Serial.print(V1.tilt);
    Serial.print(" Pan : ");
    Serial.print(V1.pan);
}

namespace motor_test{

    void test_conversion(MotorInterface& Mot);
    void test_limits(MotorInterface& Mot);
    void test_sensors(MotorInterface& Mot);
    void test_stop_to_limits(MotorInterface& Mot);
    void test_set_pose_goal(MotorInterface& Mot);
    void test_update_direction(MotorInterface& Mot);
    void test_set_home_pose(MotorInterface& Mot);
    void test_conversion(MotorInterface& Mot);
    void test_interpolate_speed(MotorInterface& Mot);
    void launch_test_motor(MotorInterface& Mot);
}


namespace test_camera
{
    void test_compute_limits(Camera& cam);
    void test_shoot(Camera& cam);
    void test_focus(Camera& cam);
    
}

namespace test_core
{
    void testAddPose(InstructionHandler& instr);

    void test_parse_state(const InstructionHandler& instr);
    void test_parse_menu(const InstructionHandler& instr);
    void test_parse_instr(const InstructionHandler& instr);

    void test_teleop_register_mode(InstructionHandler& instr);
    void test_programmed_mode(InstructionHandler& instr);
    void test_auto_mode(InstructionHandler& instr);

    void test_angle_from_pose(InstructionHandler& instr);
    void test_register_tracking(InstructionHandler& instr);
    void test_register_hyperlapse(InstructionHandler& instr);

    void test_core_function(InstructionHandler& instr);
};


#endif