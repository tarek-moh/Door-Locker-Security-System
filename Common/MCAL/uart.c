#include "uart.h"
#include "tm4c123gh6pm.h"

void UART_Init()
{
    /* 1. Enable clocks for UART2 and GPIOD */
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R2;    // Enable clock to UART2
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;    // Enable clock to PORTD (Bit 3)

    while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R3) == 0){} // Wait until GPIOD is ready

    /* --- UNLOCK PD7 --- */
    GPIO_PORTD_LOCK_R = 0x4C4F434B;   // Unlock Port D
    GPIO_PORTD_CR_R |= 0x80;          // Allow changes to PD7
    /* ------------------ */

    /* 2. Disable UART2 before configuration */
    UART2_CTL_R &= ~UART_CTL_UARTEN; 

    /* 3. Set baud rate (16 MHz clock, 9600 baud) */
    UART2_IBRD_R = 104; 
    UART2_FBRD_R = 11;

    /* 4. Configure line control (8-bit, FIFO enabled) */
    UART2_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    /* 5. Select system clock as UART clock source */
    UART2_CC_R = UART_CC_CS_SYSCLK; 

    /* 6. Enable UART2, TXE, and RXE */
    UART2_CTL_R |= (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);

    /* 7. Configure PD6 (U2RX) and PD7 (U2TX) */
    GPIO_PORTD_DEN_R   |= 0xC0;       // Enable digital function on PD6, PD7 (0xC0 = 1100 0000)
    GPIO_PORTD_AFSEL_R |= 0xC0;       // Enable alternate function on PD6, PD7
    GPIO_PORTD_AMSEL_R &= ~0xC0;      // Disable analog function

    // Configure PCTL: PD6=U2RX (val 1), PD7=U2TX (val 1)
    // Note: Use register bit-shifting if the specific macros aren't in your header
    GPIO_PORTD_PCTL_R  &= ~0xFF000000; // Clear bits for PD6 and PD7
    GPIO_PORTD_PCTL_R  |=  0x11000000; // Set PMCx to 1 for both (UART function)
}

void UART_SendByte(uint8_t data)
{
    /* Wait until transmit FIFO is not full */
    while ((UART2_FR_R & UART_FR_TXFF) != 0);
    UART2_DR_R = data;
}

uint8_t UART_ReceiveByte()
{
    /* Wait until receive FIFO is not empty (RXFE is bit 4) */
    while(UART2_FR_R & UART_FR_RXFE); 
    return (uint8_t)(UART2_DR_R & 0xFF);
}