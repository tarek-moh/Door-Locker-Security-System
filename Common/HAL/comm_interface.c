#include "comm_interface.h"

/*******************************************************************************
 *                         Functions Definitions                               *
 *******************************************************************************/

void COMM_Init(void)
{
    UART_Init();
}

void COMM_SendCommand(uint8_t cmd)
{
    UART_SendByte(cmd);
}

void COMM_SendMessage(const uint8_t *msg)
{
    while(*msg != '\0')
    {
        UART_SendByte(*msg);
        msg++;
    }
    UART_SendByte(COMM_END_MARKER);
}

uint8_t COMM_ReceiveCommand(void)
{
    return UART_ReceiveByte();
}

void COMM_ReceiveMessage(uint8_t *buffer)
{
    uint8_t i = 0;
    buffer[i] = UART_ReceiveByte();
    while(buffer[i] != COMM_END_MARKER && buffer[i] != '\r')
    {
        i++;
        buffer[i] = UART_ReceiveByte();
    }
    buffer[i] = '\0';
}
