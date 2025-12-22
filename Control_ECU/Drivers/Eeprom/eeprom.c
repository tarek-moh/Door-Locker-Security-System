#include "eeprom.h"
#include "eeprom_hw.h"

#define USED_BLOCK 1 //used block 1 to store passwords
#define PASSWORD_OFFSET 0 //used byte offset 0 to store password inside 
#define LAST_CHAR_OFFSET 3 //used byte offset 3 to store last char of the password
#define AUTOLOCKOUT_OFFSET 1 //used byte offset 1 to store auto lock value

//read the password from the eeprom/block 1 and compare it with the passed password
bool compare_Passwords(const uint8_t *entered_password){
  
  EEPROM_HW_Init(); //initialize eeprom block!!

   uint32_t stored_password = EEPROM_HW_ReadWord(USED_BLOCK, PASSWORD_OFFSET); //read full password word without incrementing to the next word address!!
   uint32_t stored_last_char = EEPROM_HW_ReadWord(USED_BLOCK, LAST_CHAR_OFFSET); //read last char of the password
    // Convert stored_password (1 word) into 4 bytes
    uint8_t stored_bytes[5];
    stored_bytes[0] = (stored_password >> 0)  & 0xFF;
    stored_bytes[1] = (stored_password >> 8)  & 0xFF;
    stored_bytes[2] = (stored_password >> 16) & 0xFF;
    stored_bytes[3] = (stored_password >> 24) & 0xFF;
    stored_bytes[4] = (stored_last_char >> 0) & 0xFF;

   for (int i = 0; i < 5; i++){
        if (stored_bytes[i] != entered_password[i]){
            return false;
        }
    }
   return true;
 
}


//replace the old password with the new passed one by writing to the old password location 
bool change_Password(const uint8_t *new_password){ 
  
  if (!new_password) return false; //pointer is null!!
  
  EEPROM_HW_Init(); //initialize eeprom block !!

  uint32_t full_password = ( new_password[0] | ((new_password[1])<<8) | ((new_password[2])<<16) | ((new_password[3])<<24));

   if (!EEPROM_HW_WriteWord(USED_BLOCK, PASSWORD_OFFSET, full_password)) {
       return false;
   }

   uint32_t last_char = new_password[4];
   if (!EEPROM_HW_WriteWord(USED_BLOCK, LAST_CHAR_OFFSET, last_char)) {
       return false;
   }

    return true; // write succeeded
}


int get_AutoLockTimeout(){
  
  EEPROM_HW_Init(); //initialize eeprom block !!
  
  uint32_t full_word = EEPROM_HW_ReadWord(USED_BLOCK, AUTOLOCKOUT_OFFSET);
  uint8_t timeout =  (full_word & 0xFF); //this should only have byte 0 !!
  
  return timeout; //compiler inertprets it as an integer 
}



bool set_AutoLockTimeout(const uint8_t lockout_time ){
  
  if((lockout_time < 5) ||(lockout_time > 30)) { return false;}
  
  EEPROM_HW_Init(); //initialize eeprom block !!

  //the stored first byte of the word is the one that has got the value of the timeout !!
  
  uint32_t oldValue = EEPROM_HW_ReadWord(USED_BLOCK, AUTOLOCKOUT_OFFSET);;
  uint32_t valueToStore = ( oldValue & 0xffffff00 ) | (lockout_time); //we can only write a whole word 

  if (!EEPROM_HW_WriteWord(USED_BLOCK, AUTOLOCKOUT_OFFSET, valueToStore)) {
      return false;
  }


  return true; // write succeeded
}