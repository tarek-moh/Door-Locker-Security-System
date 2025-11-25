#include "../../Common/HAL/comm_interface.h"
#include "string.h"


/*
    Test file for the communication interface. It contains echo tests, command 
    tests and string message tests.
    These tests are done using putty (python script) 
*/

int main()
{
  COMM_Init();
  COMM_SendMessage((uint8_t*)"Hello, from TM4C!\n");
  uint8_t puttyMsg[50];
  while(1)
  {
    uint8_t cmd = COMM_ReceiveCommand();
    switch(cmd)
    {
        case CMD_READY:
            COMM_SendCommand(CMD_ACK);
            break;
        case CMD_SEND_PASSWORD:
            uint8_t password[20];
            COMM_ReceiveMessage(password);
            if(strcmp((char*)password, "1234") == 0)
            {
                COMM_SendCommand(CMD_PASSWORD_CORRECT);
            }
            else
            {
                COMM_SendCommand(CMD_PASSWORD_WRONG);
            }
            break;
        case CMD_DOOR_UNLOCK:
            COMM_SendCommand(CMD_ACK);
            break;
        case CMD_CHANGE_PASSWORD:
            uint8_t oldPassword[20];
            uint8_t newPassword[20];
            COMM_ReceiveMessage(oldPassword);
            COMM_ReceiveMessage(newPassword);
            // Here you would normally store the new password securely
            COMM_SendCommand(CMD_ACK);
            break;
        default:
            COMM_SendCommand(CMD_UNKNOWN);
            break;
    }
  }
}
