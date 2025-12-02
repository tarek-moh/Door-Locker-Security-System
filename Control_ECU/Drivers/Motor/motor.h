#ifndef MOTOR_H_
#define MOTOR_H_
#include <stdint.h>
#include <stdbool.h>

//function declarations

void init_Motor(void); //initialize a port B -> PB1

//opening time = closing time ->use it when opning and closing the door
void set_MotorRuningTimer(int timeOfRotation ); //will use a GPT for it as required...timers interrupt fires after the elapsed autolock_timeout is done 
//fetch the lockout_timeout value from eeprom always!!

void set_lockoutTimer(int auto_lockout);

//open door
void open_door(void);

//close door 
void close_door(void);





#endif 