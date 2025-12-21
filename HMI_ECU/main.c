#include "../Common/HAL/comm_interface.h"
#include "./App/hmi.h"
#include "string.h"
#include "../Common/MCAL/tm4c123gh6pm.h"
#include "HAL/led/led.h"

void Debug_InitAndToggleRed(void)
{
    // 1. Enable Clock for Port F (Bit 5 set to 1)
    SYSCTL_RCGCGPIO_R |= 0x20;
    
    // 2. Wait until Port F is ready
    while((SYSCTL_PRGPIO_R & 0x20) == 0){};

    // 3. Set PF1 (Red LED) as Output (Bit 1 set to 1)
    // 0x02 is binary 0000 0010
    GPIO_PORTF_DIR_R |= 0x02;

    // 4. Enable Digital Function for PF1
    GPIO_PORTF_DEN_R |= 0x02;

    // 5. Toggle PF1: XOR the data register with 0x02
    GPIO_PORTF_DATA_R ^= 0x02;
}
/*
    Test file for the communication interface. It contains echo tests, command 
    tests and string message tests.
    These tests are done using putty (python script) 
*/

int main()
{
  COMM_Init();
  HMI_Init();
  
  while (COMM_ReceiveCommand() != CMD_READY) { }
  COMM_SendCommand(CMD_READY);

  DisplayConnection();

  //uint8_t isInit = COMM_ReceiveCommand();
  uint8_t volatile isInit;
  do {
      isInit = COMM_ReceiveCommand();
  } while (isInit != CMD_INIT);
  if (isInit == CMD_INIT) {
      LED_init();
       LED_setOn(LED_RED);
	uint8_t initializedPassword;
    do {
    	initializedPassword = HMI_SetupPassword();
    } while (!initializedPassword);
  }

  
  while(1)
  {
    HMI_Task();
  }
}