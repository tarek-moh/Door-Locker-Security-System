#ifndef MOTOR_H_
#define MOTOR_H_
#include <stdint.h>
#include <stdbool.h>

//function declarations

//pass the seconds needed for the door to stay open!!
void start_Motor(int auto_lockoutSeconds);

uint8_t motor_state(void); //returns door state (-1 is for closinga and maintaing closed state | 1 for openeed and maintain state ) 

#endif 