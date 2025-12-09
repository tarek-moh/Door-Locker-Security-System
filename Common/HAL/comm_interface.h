#ifndef COMM_INTERFACE_H_
#define COMM_INTERFACE_H_

#include "../MCAL/uart.h"

/*******************************************************************************
 *                         Definitions and Protocol                            *
 *******************************************************************************/

/*
 * Application-defined message protocol:
 * - Each command is 1 byte (e.g. 'O' for open door, 'C' for change pass)
 * - Followed by payload bytes if needed
 * - End of message indicated by '#\n'
 */
#define COMM_END_MARKER   '\n'


/* enumaration of command codes */
typedef enum {
    CMD_READY = 0x10,
    CMD_SEND_PASSWORD,
    CMD_PASSWORD_CORRECT,
    CMD_PASSWORD_WRONG,
    CMD_CHANGE_PASSWORD,
    CMD_DOOR_UNLOCK,
    CMD_DOOR_LOCK,
    CMD_ALARM,
    CMD_ACK,
    CMD_UNKNOWN,
    CMD_SEND_TIMEOUT

} COMM_CommandID;

/*******************************************************************************
 *                         Function Prototypes                                 *
 *******************************************************************************/


/* Initializes UART communication channel */
void COMM_Init(void);

/* Send a command (1 byte) */
void COMM_SendCommand(uint8_t cmd);

/* Send a string message terminated by COMM_END_MARKER */
void COMM_SendMessage(const uint8_t *msg);

/* Receive a command (1 byte) */
uint8_t COMM_ReceiveCommand(void);

/* Receive a string message until COMM_END_MARKER */
void COMM_ReceiveMessage(uint8_t *buffer);

#endif /* COMM_INTERFACE_H_ */
