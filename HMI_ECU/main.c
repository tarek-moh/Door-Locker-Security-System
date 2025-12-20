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
  
  DisplayConnection();
  
  while(1)
  {
    HMI_Task();
  }
}