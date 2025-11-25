#include "eeprom.h"
#include "tm4c123gh6pm.h"

#define PASSWORD_BLOCK 1 //used block 1 to store passwords
#define PASSWORD_OFFSET 0 //used byte offset 0 to store data inside 
//module level intialization not block level 
void init_Eeprom(){
  SYSCTL_RCGCEEPROM_R |=0x01;   //enable clock for whole eeprom module
  /*
    EEPROM_EEDONE_R tells the status of the eeprom : whether the EEPROM is busy , If a write/erase operation failed
    if i dont have permission to write 
    bit0 -> eeprom is working and we must wait till it becomes 0 to use it 
    bit3 -> error during erase operation 
    bit4 -> Write to a locked block attempted (EEPROM block is locked; you must unlock before writing)
  */
  while((EEPROM_EEDONE_R & (1<<0)) != 0); //wait until bit0 = 0!!
  
  SYSCTL_SREEPROM_R = 1; //reset eeprom module after clk gating to not get unpredictable behavior 
  SYSCTL_SREEPROM_R = 0; //release the reset 
  
   while((EEPROM_EEDONE_R & (1<<0)) != 0); //wait again until bit0 = 0!!
  
}

//read the password from the eeprom/block 1 and compare it with the passed password 
bool compare_Passwords(const uint8_t *entered_password){
  
  EEPROM_EEBLOCK_R = PASSWORD_BLOCK; //choose the block no 1
  EEPROM_EEOFFSET_R = PASSWORD_OFFSET; //choose the offset 0
  
   uint32_t stored_password = EEPROM_EERDWR_R; //read full password word without incrementing to the next word address!!
    // Convert stored_password (1 word) into 4 bytes
    uint8_t stored_bytes[4];
    stored_bytes[0] = (stored_password >> 0)  & 0xFF;
    stored_bytes[1] = (stored_password >> 8)  & 0xFF;
    stored_bytes[2] = (stored_password >> 16) & 0xFF;
    stored_bytes[3] = (stored_password >> 24) & 0xFF;

   for (int i = 0; i < 4; i++){
        if (stored_bytes[i] != entered_password[i]){
            return false;
        }
    }
   return true;
 
}


//replace the old password with the new passed one by writing to the old password location 
bool change_Password(const uint8_t *new_password){ 
  
  if (!new_password) return false; //pointer is null!!
  
  EEPROM_EEBLOCK_R = PASSWORD_BLOCK; //choose the block no 1
  EEPROM_EEOFFSET_R = PASSWORD_OFFSET; //choose the offset 0
  
  uint32_t full_password = ( new_password[0] | ((new_password[1])<<8) | ((new_password[2])<<16) | ((new_password[3])<<24) );
  EEPROM_EERDWR_R = full_password; //write password to memory 
  
    while((EEPROM_EEDONE_R & (1<<0)) != 0); //wait till eeprom has stopped working by checking the WORKING bit  
    
    // check for errors
    if (EEPROM_EEDONE_R & (EEPROM_EEDONE_WKCOPY | EEPROM_EEDONE_WKERASE | EEPROM_EEDONE_NOPERM)) {
        return false; // write failed
    }

    return true; // write succeeded
}