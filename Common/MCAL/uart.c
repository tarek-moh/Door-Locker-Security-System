#include "uart.h"
#include "tm4c123gh6pm.h"


void UART_Init()
{
    /* 1. Enable clocks for UART0 and GPIOA */
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;    // Enable clock to UART0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;    // Enable clock to PORTA

    while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0) == 0){} // Wait until GPIOA is ready

    /* 2. Disable UART0 before configuration */
    UART0_CTL_R &= ~UART_CTL_UARTEN; // Disable UART0 (UARTEN = 0)

    /* 3. Set baud rate (assuming 16 MHz system clock and 9600 baud) */
    // Baud Rate Divisor = System Clock / (16 * Baud Rate)
    // BRD = 16,000,000 / (16 * 9600) = 104.1666...
    UART0_IBRD_R = 104; // Integer part = 104
    // Fractional part = integer(0.1666... * 64 + 0.5) = 11
    UART0_FBRD_R = 11;

    /* 4. Configure line control for:
     * - 8-bit word length
     * - No parity
     * - 1 stop bit
     * - FIFO enabled
     */
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    /* 5. Select system clock as UART clock source */
    UART0_CC_R = UART_CC_CS_SYSCLK; // Use system clock

    /* 6. Enable UART0, TXE (Transmit Enable), and RXE (Receive Enable) */
    UART0_CTL_R |= (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);

    /* 7. Configure PA0 (U0RX) and PA1 (U0TX) pins for UART function */
    GPIO_PORTA_DEN_R   |= 0x03; // Enable digital function on PA0, PA1
    GPIO_PORTA_AFSEL_R |= 0x03; // Enable alternate function on PA0, PA1
    // Clear current PCTL settings for PA0 and PA1
    GPIO_PORTA_PCTL_R  &= ~(GPIO_PCTL_PA1_M | GPIO_PCTL_PA0_M);
    // Configure PA0 as U0RX (1) and PA1 as U0TX (1)
    GPIO_PORTA_PCTL_R  |= (GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX);
    GPIO_PORTA_AMSEL_R &= ~0x03; // Disable analog function on PA0, PA1
}

void UART_SendByte(uint8_t data)
{
    /* Wait until transmit FIFO is not full */
    while ((UART0_FR_R & UART_FR_TXFF) != 0);
    UART0_DR_R = data;
}

uint8_t UART_ReceiveByte()
{
  /* Wait until receive FIFO is not empty */
  while((UART0_FR_R & (1<<4)));
  return (uint8_t)(UART0_DR_R & 0xFF);
}
