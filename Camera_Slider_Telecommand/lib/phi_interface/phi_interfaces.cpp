#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <phi_interfaces.h>

// Static member variable initialization
unsigned int multiple_button_input::buttons_hold_time=buttons_hold_time_def;
unsigned int multiple_button_input::buttons_debounce_time=buttons_debounce_time_def;
unsigned int multiple_button_input::buttons_dash_threshold=buttons_dash_threshold_def;
unsigned int multiple_button_input::buttons_repeat_time=buttons_repeat_time_def;
unsigned int multiple_button_input::buttons_dash_time=buttons_dash_time_def;

unsigned long multiple_button_input::t_last_action=0;


//Keypad class member functions:
/*
 __  ___  ___________    ____ .______      ___       _______  
|  |/  / |   ____\   \  /   / |   _  \    /   \     |       \ 
|  '  /  |  |__   \   \/   /  |  |_)  |  /  ^  \    |  .--.  |
|    <   |   __|   \_    _/   |   ___/  /  /_\  \   |  |  |  |
|  .  \  |  |____    |  |     |  |     /  _____  \  |  '--'  |
|__|\__\ |_______|   |__|     | _|    /__/     \__\ |_______/ 
*/
/**
 * \details Outputs the name of the last sensed key or NO_KEY. If all you want is to  sense a key press, use getKey instead. You can use this in conjunction with get_status to sense if a key is held.
 * \return it returns the name of the last sensed key or NO_KEY. This key may not be currently pressed.
 */ 
byte phi_keypads::get_sensed()
{
  if (button_sensed==NO_KEYs) return NO_KEY;
  else return key_names[button_sensed];
}

/**
 * \details Return status of the sensed key. If there is no sensed key, this return should not be used.
 * \return It returns status of the sensed key.
 */
byte phi_keypads::get_status()
{
  return button_status;
}

/**
 * \details This is the public method to get a key press from the keypad. The key press is translated into key_names or NO_KEY.
 * This function is inherited from multiple_button_inputs. All Keypad subclasses such as phi_matrix_keypads and phi_analog_keypads share this code.
 * Since all multiple_button_inputs devices have this method, you can treat all of them as multiple_button_inputs and call this method to get a key press.
 * This is the function you should call to sense key presses. 
 * It is only few lines of code and is generic enough for all phi_keypads.
 * \return It returns the name of the key that is pressed down.
 */
byte phi_keypads::getKey()
{
  byte key=scanKeypad();
  if (key==NO_KEYs) key=NO_KEY;
  else key=key_names[key];
  return key;
}

/**
 * \details This routine uses senseAll to scan the keypad, use debouncing to update button_sensed and button_status.
 * This function is not intended to be call by arduino code but called within the library instead.
 * If all you want is a key press, call getKey.
 * The getKey calls this function and translates keypress from scan code (0 to max_key-1) into characters or key names.
 * \return This function only returns scan code (0 to max_key-1).
 */
byte phi_keypads::scanKeypad()
{
  byte button_pressed=sense_all();
  
  switch (button_status)
  {
    case buttons_up:
    if (button_pressed!=NO_KEYs)
    {
      button_sensed==button_pressed;
      button_status_t=millis();
      button_status=buttons_debounce;
    }
    else button_sensed=NO_KEYs;
    break;
    
    case buttons_debounce:
    if (button_pressed!=NO_KEYs)
    {
      if (button_sensed==button_pressed)
      {
        if (millis()-button_status_t>buttons_debounce_time)
        {
          button_status=buttons_pressed;
          button_status_t=millis();
          t_last_action=button_status_t;
          return button_sensed;
        }
      }
      else
      {
        button_status_t=millis();
        button_sensed=button_pressed;
      }
    }
    else
    {
      button_status=buttons_up;
      button_sensed=NO_KEYs;
    }
    break;
    
    case buttons_pressed:
    if (button_pressed!=NO_KEYs)
    {
      if (button_sensed==button_pressed) button_status=buttons_down;
      else button_status=buttons_debounce;
      button_status=buttons_down;
    }
    else button_status=buttons_released;
    button_status_t=millis();
    break;
    
    case buttons_down:
    if (button_sensed==button_pressed)
    {
      if (millis()-button_status_t>buttons_hold_time)
      {
        button_status=buttons_held;
        button_status_t=millis();
      }
    }
    else
    {
      button_status=buttons_released;
      button_status_t=millis();
    }
    break;
    
    case buttons_released:
    if (button_pressed==NO_KEYs) button_status=buttons_up;
    else
    {
      button_status=buttons_debounce;
      button_sensed=button_pressed;
      button_status_t=millis();
    }
    break;
    
    case buttons_held:
    if (button_sensed!=button_pressed)
    {
      button_status=buttons_released;
      button_status_t=millis();
      return button_sensed;
    }
    else if (millis()-button_status_t>buttons_repeat_time)
    {
      button_status_t=millis();
      return button_sensed;
    }
    break;
  }
  return NO_KEYs;
}

//Joystick class member functions
/*
       __    ______   ____    ____  _______.___________. __    ______  __  ___
      |  |  /  __  \  \   \  /   / /       |           ||  |  /      ||  |/  /
      |  | |  |  |  |  \   \/   / |   (----`---|  |----`|  | |  ,----'|  '  /
.--.  |  | |  |  |  |   \_    _/   \   \       |  |     |  | |  |     |    <
|  `--'  | |  `--'  |     |  | .----)   |      |  |     |  | |  `----.|  .  \
 \______/   \______/      |__| |_______/       |__|     |__|  \______||__|\__\
*/
/**
 * \details A joystick has two potentiometers and provides the x and y axis locations with the wiper pins.
 * If the shaft is clickable, declare the button in a phi_button_groups object.
 * \param na This is the name of (or pointer to) a char array that stores the names corresponding to each directional key press. The array starts with the top left direction and iterates through top, top right, etc. The middle position is filled with NO_KEYs, not NO_KEY.
 * \param sp This is the name of (or pointer to) a byte array that stores all analog pins used by the joystick.
 * \param dp This is the name of (or pointer to) an integer array that stores the analog values of each button press (stick push). The array must be sorted from small to big then from one axis to the next.
 * \param th This stores the threshold of matching the joystick with a directional key.
 */
phi_joysticks::phi_joysticks(char *na, byte *sp, int * dp, int th)
{
  device_type=Joy_stick;
  key_names=na; // Translated names of the keys, such as '0'.
  mySensorPins=sp; // Row pins
  values=dp; // Points to divider value array.
  rows=2;
  columns=3;
  threshold=th;
  button_sensed=NO_KEYs; // This indicates which button is sensed or 255 if no button is sensed.
  button_status=buttons_up; // This indicates the status of the button if button_sensed is not 255.
  button_status_t=millis(); // This is the time stamp of the sensed button first in the status stored in button_status.
  
  for (int j=0;j<rows;j++) // Setting sensing rows to input and disabling internal pull-up resistors.
  {
    pinMode(mySensorPins[j],INPUT);
    digitalWrite(mySensorPins[j],LOW);
  }
}

/**
 * \details This is the most physical layer of the phi_joysticks. Senses all input pins for a valid status.
 * This function is not intended to be call by arduino code but called within the library instead.
 * If all you want is a key press, call getKey.
 * \return It returns the button scan code (0-max_button-1) that is pressed down or NO_KEYs if no button is pressed down. The return is 0-based so the value is 0-15 if the array has 16 buttons.
 */
byte phi_joysticks::sense_all()
{
  byte diff[2];
  diff[0]=NO_KEYs;
  diff[1]=NO_KEYs;
  for (byte i=0;i<rows;i++)
  {
    axis_vals[i]=analogRead(mySensorPins[i]);
    delay(5);
  }
  for (byte j=0;j<rows;j++)
  {
    for (byte i=0;i<columns;i++)
    {
      if(abs(values[j*columns+i]-axis_vals[j])<threshold) diff[j]=i; // Find the difference between analog read and stored values.
    }
  }
  if ((diff[0]==NO_KEYs)||(diff[1]==NO_KEYs)||((diff[0]==columns/2)&&(diff[1]==columns/2))) return NO_KEYs; // returns the button pressed if neither axis is in the middle.
  else return (diff[0]*columns+diff[1]);
}


//Button arrays class member functions
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
/**
 * \details Button group is a class that senses a group of push buttons. This class is preferred compared with phi_buttons class.
 * You can assign names to each button so when the button is pressed, the name is returned such as '1', or 'A'.
 * All buttons should connect arduino pins to GND. Internal pull-up resistors are automatically enabled.
 * \param na This is the name of (or pointer to) a char array that stores the names corresponding to each key press.
 * \param sp This is the name of (or pointer to) a byte array that stores all arduino pins used by each button. If you have 4 buttons, the array has 4 elements.
 * \param r This is the number of buttons in this group.
 */
phi_button_groups::phi_button_groups(char *na, byte * sp,byte* _logic, byte r) // button_groups constructor
{
  device_type=Button_group;

  key_names=na; // Translated names of the keys, such as '0'.
  mySensorPins=sp; // Row pins
  rows=r;
  logic = _logic;
  button_sensed=NO_KEYs; // This indicates which button is sensed or 255 if no button is sensed.
  button_status=buttons_up; // This indicates the status of the button if button_sensed is not 255.
  button_status_t=millis(); // This is the time stamp of the sensed button first in the status stored in button_status.
  
  for (int j=0;j<rows;j++) // Setting sensing rows to input and enabling internal pull-up resistors.
  {
    pinMode(mySensorPins[j],INPUT_PULLUP);
    digitalWrite(mySensorPins[j],HIGH);
  }
}

// /**
//  * \details This is the most physical layer of the phi_keypads. Senses all input pins for a valid status. The scanKeypad calls this function and interprets the return into status of the key.
//  * This function is not intended to be call by arduino code but called within the library instead.
//  * If all you want is a key press, call getKey.
//  * \return It returns the button scan code (0-max_button-1) that is pressed down or NO_KEYs if no button is pressed down. The return is 0-based so the value is 0-15 if the array has 16 buttons.
//  */
byte phi_button_groups::sense_all()
{
  for (byte j=0;j<rows;j++)
  {
    buttonBits=digitalRead(mySensorPins[j]); // Sensing button, using buttonBits as a temp variable.
    if ((buttonBits==LOW && !logic[j]) || (buttonBits==HIGH && logic[j]))
    {
      return (j); // returns the button pressed
    }
  }
  return NO_KEYs; // no buttons pressed
}
