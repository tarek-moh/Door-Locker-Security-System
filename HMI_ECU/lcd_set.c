#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "lcd_set.h"

void delayMs(int n) {
    int i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 3180; j++);
}

void LCD_sendNibble(uint8_t nibble) {
    GPIOB_DATA_R = (GPIOB_DATA_R & 0x0F) | (nibble << 4);
    GPIOB_DATA_R |= 0x02; // EN HIGH
    delayMs(1);
    GPIOB_DATA_R &= ~0x02; // EN LOW
    delayMs(1);
}

void LCD_command(uint8_t cmd) {
    GPIOB_DATA_R &= ~0x01; // RS=0
    LCD_sendNibble(cmd >> 4);
    LCD_sendNibble(cmd & 0x0F);
}

void LCD_data(uint8_t data) {
    GPIOB_DATA_R |= 0x01; // RS=1
    LCD_sendNibble(data >> 4);
    LCD_sendNibble(data & 0x0F);
}

void LCD_init() {
    SYSCTL_RCGCGPIO_R |= 0x02; // Port B
    GPIOB_DIR_R |= 0xF3;
    GPIOB_DEN_R |= 0xF3;

    delayMs(20);
    LCD_command(0x28);
    LCD_command(0x06);
    LCD_command(0x01);
    LCD_command(0x0F);
}

void LCD_print(char *str) {
    while (*str) {
        LCD_data(*str++);
    }
}
