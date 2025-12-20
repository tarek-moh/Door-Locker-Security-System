#include "../Common/HAL/comm_interface.h"
#include "./App/hmi.h"
#include "string.h"

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

  uint8_t isInit = COMM_ReceiveCommand();
  if (isInit == CMD_INIT) {
	uint8_t initializedPassword;
    do {
    	initializedPassword = HMI_SetupPassword();
    } while (!initializedPassword);
  }

  DisplayConnection();
  
  while(1)
  {
    HMI_Task();
  }
}