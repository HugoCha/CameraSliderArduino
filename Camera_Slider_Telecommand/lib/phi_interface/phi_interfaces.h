/** \file
 *  \brief     This is the first official release of the phi_interfaces library.
 *  \details   This library unites buttons, rotary encoders and several types of keypads libraries under one library, the phi_interfaces library, for easy of use.
 *  This is the first official release. All currently supported input devices are buttons, matrix keypads, rotary encoders, analog buttons, 2-axis joysticks, serial keypads, and types 1 and 2 liudr keypads.
 *  User is encouraged to obtain compatible hardware from liudr or is solely responsible for converting it to work on other shields or configurations.
 *  \author    Dr. John Liu
 *  \version   1.6.0
 *  \date      05/28/2015
 *  \pre       Compatible with Arduino IDE 1.6.
 *  \bug       Not tested on Arduino IDE higher than 1.6.0!
 *  \warning   PLEASE DO NOT REMOVE THIS COMMENT WHEN REDISTRIBUTING! No warranty!
 *  \copyright Dr. John Liu. GNU GPL V 3.0.
 *  \par Contact
 * Obtain the documentation or find details of the phi_interfaces, phi_prompt TUI library, Phi-2 shield, and Phi-panel hardware or contact Dr. Liu at:
 *
 * <a href="http://liudr.wordpress.com/phi_interfaces/">http://liudr.wordpress.com/phi_interfaces/</a>
 *
 * <a href="http://liudr.wordpress.com/phi-panel/">http://liudr.wordpress.com/phi-panel/</a>
 *
 * <a href="http://liudr.wordpress.com/phi_prompt/">http://liudr.wordpress.com/phi_prompt/</a>
 *
 * <a href="http://liudr.wordpress.com/phi-2-shield/">http://liudr.wordpress.com/phi-2-shield/</a>
 *
 *  \par Updates
 * 05/28/2015: Released under GNU GPL V 3.0 Yeah!
 * 06/25/2014: Finished coding liudr_rotary_encoders_a and liudr_rotary_encoders_d classes with tests.
 * 06/18/2014: Started to code liudr_rotary_encoders_a for the OSPL V 2.1.X
 * 02/10/2014: Added phi_liudr_keypads_2 for the new phi-panels.
 * 11/29/2012: Changed debounce time from 50ms to 20ms to speed up rotary encoder.
 * 11/24/2012: Added analog_difference define to control the maximal analog difference when comparing. The value was 10.
 * 02/05/2012: Added support for 2-axis joystick as a directional keypad and example code.
 * 01/16/2012: Original code released.
*/

#ifndef phi_interfaces_h
#define phi_interfaces_h
#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

//Device types:
#define Single_button 1                 ///< (Not used in this library) Single buttons need to connect an arduino pin to GND.
#define Keypad 2                        ///< Generic keypad
#define Button_group 5                  ///< A group of arduino pins, each connected to a single button. Single buttons need to connect an arduino pin to GND.
#define Serial_keypad 9                 ///< Phi-panel serial LCD keypad as input.
#define Joy_stick 10                    ///< Phi_joy_stick as digital input with 8 directional keys.

//Button status
#define buttons_up 0        ///< Non-transitional button status
#define buttons_pressed 1   ///< Transitional button status
#define buttons_down 2      ///< Non-transitional button status
#define buttons_held 3      ///< Non-transitional button status
#define buttons_released 4  ///< Transitional button status
#define buttons_debounce 5  ///< One needs to wait till debounce status is over to become pressed status to confirm a press.

//Operating parameters
#define buttons_hold_time_def 1000      ///< Default key down time needed to be considered the key is held down
#define buttons_debounce_time_def 25    ///< Default key down time needed to be considered the key is not bouncing anymore
#define buttons_dash_threshold_def 10   ///< Default key down time needed to be considered the key is held down long enough to repeat in a dash speed
#define buttons_repeat_time_def 200     ///< Default delay between repeating of a held key
#define buttons_dash_time_def 50        ///< Default delay between dash repeating of a held key

//Internal and external "NO KEY" return values
#define NO_KEYs 255         ///< This is no key in scan code, internal to the library.
#define NO_KEY 0            ///< This is no key that the library outputs to a caller, to be compatible with keypad.h

//Analog keypad specific defines
#define analog_difference 12			///< Analog reading maximal difference when comparing with expected analog value.
#define analog_difference_2 12			///< Used in liudr pad V 2. Analog reading maximal difference when comparing with expected analog value.

//Pure virtual base classes (interfaces) start here
/*
.___  ___.  __    __   __      .___________. __  .______    __       _______
|   \/   | |  |  |  | |  |     |           ||  | |   _  \  |  |     |   ____|
|  \  /  | |  |  |  | |  |     `---|  |----`|  | |  |_)  | |  |     |  |__
|  |\/|  | |  |  |  | |  |         |  |     |  | |   ___/  |  |     |   __|
|  |  |  | |  `--'  | |  `----.    |  |     |  | |  |      |  `----.|  |____
|__|  |__|  \______/  |_______|    |__|     |__| | _|      |_______||_______|
*/
/** \brief Virtual base class for inputs that contain multiple keys.
 * \details This is a virtual base class meant to be inherited by child classes. You cannot instantiate any objects from this class.
 * This class is inherited by classes to handle matrix keypads, PS/2 keyboards (planned), rotary encoders, analog buttons, button groups, 2-axis joysticks.
 * This class handles anything that requires the program to not only sense the status of a digital or analog pin status, but also interpret the input and output keys pressed.
*/
class multiple_button_input{
  public:
/// This stores the type of the device such as rotary encoder or keypad etc.
  byte device_type;
/** \brief This function is responsible for sensing the input for key press and update status.
 * \details This function is responsible for sensing the input for key press and update status. Each child class implements this method to translate physical status changes into named buttons.
*/
  virtual byte getKey()=0;
/// This returns device type. See device type defs.
  virtual byte get_device_type() {return device_type;};
 /// This should be run after getKey to get the up-to-date result.
  virtual byte get_status()=0;
/// This should be run after getKey to get the up-to-date result.
  virtual byte get_sensed()=0;
/// This sets how long the button needs to be held before it repeats.
  virtual void set_hold(unsigned int ht) {buttons_hold_time=ht;};
/// This sets how long the button needs to be held before it is considered pressed.
  virtual void set_debounce(unsigned int dt) {buttons_debounce_time=dt;};
/// This sets how long the button needs to be held before it repeats rapidly.
  virtual void set_dash_threshold(unsigned int dt) {buttons_dash_threshold=dt;};
/// This sets how often the button press repeats after being held.
  virtual void set_repeat(unsigned int rt) {buttons_repeat_time=rt;};
/// This sets how often the button press rapidly repeats after being held.
  virtual void set_dash(unsigned int dt) {buttons_dash_time=dt;};

  protected:
  static unsigned long t_last_action;           ///< This stores the last time any real keypad was active. You may use this to implement sleeping mode.
  static unsigned int buttons_hold_time;        ///< Key down time needed to be considered the key is held down
  static unsigned int buttons_debounce_time;    ///< Key down time needed to be considered the key is not bouncing anymore
  static unsigned int buttons_dash_threshold;   ///< Key down time needed to be considered the key is held down long enough to repeat in a dash speed
  static unsigned int buttons_repeat_time;      ///< Delay between repeating of a held key
  static unsigned int buttons_dash_time;        ///< Delay between dash repeating of a held key
};


/*
 __  ___  ___________    ____ .______      ___       _______
|  |/  / |   ____\   \  /   / |   _  \    /   \     |       \
|  '  /  |  |__   \   \/   /  |  |_)  |  /  ^  \    |  .--.  |
|    <   |   __|   \_    _/   |   ___/  /  /_\  \   |  |  |  |
|  .  \  |  |____    |  |     |  |     /  _____  \  |  '--'  |
|__|\__\ |_______|   |__|     | _|    /__/     \__\ |_______/
*/
/** \brief virtual class for all keypad subclasses
 * \details This class provides the hierarchy for actual keypad classes to inherit from. It provides common high-level function codes.
 * These function codes, coupled with the lower level function code of each inheriting child class, completes the translation from sensing physical pins to outputting named buttons with mapping array.
 * The function hierarchy is getKey()<---scanKeypad()<---sense_all().
 * The sense_all reads digital pins for input.
 * The scanKeypad turns these inputs into status changes for keys and provide scan code of the pressed key. It handles status change including debouncing and repeat.
 * The getKey translates the key press from scan code (0 to max_key-1) into named keys with the mapping array.
*/
class phi_keypads:public multiple_button_input {
  public:
  byte keyboard_type;               ///< This stores the type of the keypad so a caller can use special functions for specific keypads.
  byte getKey() override;                    ///< Returns the key corresponding to the pressed button or NO_KEY.
  unsigned long button_status_t;    ///< This is the time stamp of the sensed button first in the status stored in button_status.

  virtual byte get_sensed() override;        ///< Get sensed button name. Replace this in children class if needed.
  virtual byte get_status() override;        ///< Get status of the button being sensed. Replace this in children class if needed.

  protected:
  byte rows;                ///< Number of rows on a keypad. Rows are input pins. In analog keypads, each row pin is an analog pin.
  byte columns;             ///< Number of columns on a keypad. Columns are output pins when the column is addressed and tri-stated when the column is not addressed. In analog keypads, column represents number of buttons connected to each analog pin.
  byte buttonBits;          ///< This is the button bits. It's a temporary variable.
  byte button_sensed;       ///< This indicates which button is sensed or 255 if no button is sensed.
  byte button_status;       ///< This indicates the status of the button if button_sensed is not 255.
  byte * mySensorPins;      ///< Pointer to array of pins. Each subclass has a different convention of what pins are used, usually rows are followed by columns.
  char * key_names;         ///< Pointer to array of characters. Each key press is translated into a name from this array such as '0'.

  byte scanKeypad();        ///< Updates status of the keypad with button_sensed and button_status to provide information to getKey
/// This senses all input pins.
  virtual byte sense_all()=0;
};

/*
       __    ______   ____    ____  _______.___________. __    ______  __  ___
      |  |  /  __  \  \   \  /   / /       |           ||  |  /      ||  |/  /
      |  | |  |  |  |  \   \/   / |   (----`---|  |----`|  | |  ,----'|  '  /
.--.  |  | |  |  |  |   \_    _/   \   \       |  |     |  | |  |     |    <
|  `--'  | |  `--'  |     |  | .----)   |      |  |     |  | |  `----.|  .  \
 \______/   \______/      |__| |_______/       |__|     |__|  \______||__|\__\
*/
/** \brief class for 2-axis joy sticks
 * \details This class provides support to sense a 2-axis joy stick, with either analog output on each axis, or digital output of 8 directional keys.
 * These function codes, coupled with the lower level function code of each inheriting child class, completes the translation from sensing physical pins to outputting named buttons with mapping array.
 * The function hierarchy is getKey()<---scanKeypad()<---sense_all().
 * The sense_all reads digital pins for input.
 * The scanKeypad turns these inputs into status changes for keys and provide scan code of the pressed key. It handles status change including debouncing and repeat.
 * The getKey translates the key press from scan code (0 to max_key-1) into named keys with the mapping array.
*/
class phi_joysticks:public phi_keypads {
  public:
  phi_joysticks(char *na, byte *sp, int * dp, int th); ///< Constructor for joystick
//   byte keyboard_type;               ///< This stores the type of the keypad so a caller can use special functions for specific keypads.
  int get_x(){return axis_vals[0];} ///< Returns x axis value of the joystick
  int get_y(){return axis_vals[1];} ///< Returns y axis value of the joystick
//   unsigned long button_status_t;    ///< This is the time stamp of the sensed button first in the status stored in button_status.

  protected:
  int axis_vals[2];         ///< This stores the x and y axis values read from analog pin
  int threshold;            ///< This stores the threshold of matching the joystick with a directional key.
  int * values;             ///< This pointer points to an integer array with values of analog inputs. The number of values is equal to the number of axis times 3. The array starts with the value of the first axis, when it is pushed up, then the center value of this axis, then the value of this axis when it is pushed down.
  byte sense_all() override; ///< This senses all input pins.
};



/*
.______    __    __  .___________.___________.  ______   .__   __.
|   _  \  |  |  |  | |           |           | /  __  \  |  \ |  |
|  |_)  | |  |  |  | `---|  |----`---|  |----`|  |  |  | |   \|  |
|   _  <  |  |  |  |     |  |        |  |     |  |  |  | |  . `  |
|  |_)  | |  `--'  |     |  |        |  |     |  `--'  | |  |\   |
|______/   \______/      |__|        |__|      \______/  |__| \__|

  _______ .______        ______    __    __  .______        _______.
 /  _____||   _  \      /  __  \  |  |  |  | |   _  \      /       |
|  |  __  |  |_)  |    |  |  |  | |  |  |  | |  |_)  |    |   (----`
|  | |_ | |      /     |  |  |  | |  |  |  | |   ___/      \   \
|  |__| | |  |\  \----.|  `--'  | |  `--'  | |  |      .----)   |
 \______| | _| `._____| \______/   \______/  | _|      |_______/
*/
/** \brief a class for a group of buttons
 * \details Collection of all single buttons into a group and handled as a keypad so each button push is translated into a named key value such as '1'.
 * The pointer to pins has no column or row lines. Each pin is connected to one button.
 * This is the way to go if you want to start small with few buttons and intend to expand your interface into more buttons and add rotary encoders and keypads.
 * Using this class instead of phi_buttons class also gives you a virtual layer, where you can develop your project without any buttons or keypads and simulate such input with serial
 * You should only use the phi_buttons class if you are happy with just a few buttons and don't intend to expand your interface into mixtures of keypads, rotary encoders etc.
*/
class phi_button_groups: public phi_keypads{
  public:
  phi_button_groups(char *na, byte * sp, byte* _logic, byte r); ///< Constructor for phi_button_groups

  protected:
  byte sense_all() override;         ///< This senses all input pins.

  private:
    byte* logic;
};



#endif
