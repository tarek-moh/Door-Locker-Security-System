#ifndef UART_H_
#define UART_H_

#include <stdint.h>

/* ------------- Functions Protoypes -------------- */
void UART_init();
void UART_SendByte(uint8_t data);
uint8_t UART_ReceiveByte();


#endif