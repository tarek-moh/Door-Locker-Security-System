#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <stdint.h>

void LCD_I2C_Init(void);
void LCD_I2C_Clear(void);
void LCD_I2C_SetCursor(uint8_t row, uint8_t col);
void LCD_I2C_WriteChar(char c);
void LCD_I2C_WriteString(const char *str);

#endif