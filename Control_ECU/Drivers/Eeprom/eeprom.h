#ifndef EEPROM_H_
#define EEPROM_H_
#include <stdint.h>
#include <stdbool.h>

//function declarations

//used block no.1 & offsets 0&1 
void init_Eeprom(void); //module level intialization not block level 
bool compare_Passwords(const uint8_t *entered_password); //compares sent password with the stored one!!
bool change_Password(const uint8_t *new_password); //changes stored password with the passed new one 
int get_AutoLockTimeout(); //returns the value of timeout !!
bool set_AutoLockTimeout(const uint8_t lockout_time ); //sets a new auto lockout time 


#endif