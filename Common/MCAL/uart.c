#include "uart.h"
#include "tm4c123gh6pm.h"

void UART_Init()
{
    /* 1. Enable clocks for UART0 and GPIOA */
    SYSCTL->RCGCUART |= (1 << 0);   // Enable clock to UART0
    SYSCTL->RCGCGPIO |= (1 << 0);   // Enable clock to PORTA    
  
    while((SYSCTL->PRGPIO & 0x01) == 0){}  // Wait until GPIOA is ready
    
    /* 2. Disable UART0 before configuration */
    UART0->CTL &= ~(1 << 0);   // Disable UART0 (UARTEN = 0)

    /* 3. Set baud rate */
    UART0->IBRD = 104;   // Integer part (16 MHz / (16 * 9600)) = 104.1666
    UART0->FBRD = 11;    // Fractional part = 0.1666 * 64 + 0.5 = 11
    
    /* 4. Configure line control for 8N1 (8-bit, no parity, 1 stop bit) */
    UART0->LCRH = (0x3 << 5);  // WLEN=11 (8-bit), FEN=0 (FIFO disabled for now) (NOT FINAL!)
    
     /* 5. Select system clock as UART clock source */
    UART0->CC = 0x0;  // Use system clock (16 MHz)
    
    /* 6. Enable UART0, TX, and RX */
    UART0->CTL = (1 << 0) | (1 << 8) | (1 << 9);  // UARTEN, TXE, RXE
    
    /* 7. Configure PA0 and PA1 for UART0 */
    GPIOA->DEN   |= 0x03;     // Enable digital function on PA0, PA1
    GPIOA->AFSEL |= 0x03;     // Enable alternate function on PA0, PA1
    GPIOA->PCTL  &= ~0x000000FF; // Clear PCTL bits for PA0, PA1
    GPIOA->PCTL  |= 0x00000011;  // Configure PA0->U0Rx, PA1->U0Tx
    GPIOA->AMSEL &= ~0x03;    // Disable analog on PA0, PA1
        
}
