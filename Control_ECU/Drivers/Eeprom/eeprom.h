#ifndef EEPROM_H_
#define EEPROM_H_
#include <stdint.h>
#include <stdbool.h>

//function declarations

void init_Eeprom(void); //module level intialization not block level 
bool compare_Passwords(const uint8_t *entered_password); //compares sent password with the stored one!!
bool change_Password(const uint8_t *new_password); //changes stored password with the passed new one 
 


#endif