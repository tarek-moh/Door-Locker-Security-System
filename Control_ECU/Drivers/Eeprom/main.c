//#include "../../Control_ECU/Drivers/Buzzer/buzzer.h"
//#include "buzzer.h"
#include "eeprom.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
/*
    Test file for the communication interface. It contains echo tests, command 
    tests and string message tests.
    These tests are done using putty (python script) 
*/

void init_LEDs(void) {
    SYSCTL_RCGCGPIO_R |= (1 << 5);        // Enable clock for Port F
    while(!(SYSCTL_PRGPIO_R & (1 << 5))); // Wait until Port F is ready

    GPIO_PORTF_DIR_R |= 0x0E;  // PF1, PF2, PF3 as output
    GPIO_PORTF_DEN_R |= 0x0E;  // Digital enable PF1, PF2, PF3
    GPIO_PORTF_DATA_R &= ~0x0E; // Turn all LEDs off initially
}

void toggle_LED(uint8_t led_pin) {
    GPIO_PORTF_DATA_R ^= led_pin;  // Toggle specific LED
}

int main(void){
 // init_Buzzer();
// __asm("CPSIE I");  // set the I-bit in PRIMASK ? enables global interrupts
 // toggle_Buzzer();
  
  //testing reading from eeprom 
  
 // uint8_t entered_password[4] = {0x12, 0x34, 0x56, 0x78}; //mismatched password 
  uint8_t entered_password[4] = {0xAA, 0xBB, 0xCC, 0xDD};
  init_Eeprom();
  init_LEDs(); 
  bool compare_flag = compare_Passwords(entered_password);
  
   if(compare_flag){
        //Password matched
       toggle_LED(1 << 2);
    } else {
        // Password did not match
      toggle_LED(1 << 1);  
    }
   
   
    //testing writing in eeprom 
   // uint8_t new_password[4] = {0xAA, 0xBB, 0xCC, 0xDD};
   // bool reset_flag = change_Password(new_password);
    
   // if(reset_flag){
       // Password successfully changed toggle blue led 
    //   toggle_LED(1 << 2);
      
   // } else {
      // Write failed toggle red led 
  //    toggle_LED(1 << 1);    
   // }
    
   // while(1); //stop
  
  
}
