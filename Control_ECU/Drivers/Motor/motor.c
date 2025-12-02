#include "motor.h"
#include "tm4c123gh6pm.h"


//intialize PB for 
void init_Motor(void){ 
  //********************PORT B init****************************************//
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
  while((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R1) == 0){} //wait till port is ready
  
  GPIO_PORTF_DIR_R |= (1<<1); //set PB1 as output pin
  GPIO_PORTF_DEN_R |= (1<<1); //digital enable for PB1
  
}



