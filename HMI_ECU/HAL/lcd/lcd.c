#include "tm4c123gh6pm.h"
#include "lcd.h"

// PCF8574 Address
#define LCD_I2C_ADDR   0x27

// Control bits
#define LCD_BACKLIGHT  0x08
#define EN             0x04
#define RW             0x02
#define RS             0x01

// ---------- I2C LOW LEVEL ----------
void I2C0_Init(void)
{
    SYSCTL_RCGCI2C_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= 0x02;

    GPIO_PORTB_AFSEL_R |= 0x0C;
    GPIO_PORTB_ODR_R   |= 0x08;
    GPIO_PORTB_DEN_R   |= 0x0C;
    GPIO_PORTB_AMSEL_R &= ~0x0C;
    GPIO_PORTB_PCTL_R  = (GPIO_PORTB_PCTL_R & ~0xFF00) | 0x3300;

    I2C0_MCR_R = 0x10;
    I2C0_MTPR_R = 7;   // ? Correct
}

void I2C0_SendByte(uint8_t data)
{
    I2C0_MSA_R = (LCD_I2C_ADDR << 1);
    I2C0_MDR_R = data;
    I2C0_MCS_R = 0x07;

    while (I2C0_MCS_R & 1);

    if (I2C0_MCS_R & 0x02) // ERROR
    {
        I2C0_MCS_R = 0x04;
    }
}

// ---------- LCD LOW LEVEL ----------
void LCD_PulseEnable(uint8_t data)
{
    I2C0_SendByte(data | EN | LCD_BACKLIGHT);
    for (volatile int i = 0; i < 3000; i++);
    I2C0_SendByte((data & ~EN) | LCD_BACKLIGHT);
    for (volatile int i = 0; i < 3000; i++);
}

void LCD_Write4Bits(uint8_t value)
{
    I2C0_SendByte(value | LCD_BACKLIGHT);
    LCD_PulseEnable(value);
}

// ---------- HIGH LEVEL COMMANDS ----------
void LCD_SendCommand(uint8_t cmd)
{
    LCD_Write4Bits(cmd & 0xF0);
    LCD_Write4Bits((cmd << 4) & 0xF0);
}

void LCD_SendData(uint8_t data)
{
    LCD_Write4Bits((data & 0xF0) | RS);
    LCD_Write4Bits(((data << 4) & 0xF0) | RS);
}

// ---------- PUBLIC FUNCTIONS ----------
void LCD_I2C_Init(void)
{
    I2C0_Init();

    for (volatile int d=0; d<50000; d++); // Wait LCD boot

    LCD_SendCommand(0x33);  
    LCD_SendCommand(0x32);  
    LCD_SendCommand(0x28);  // 4-bit, 2 line
    LCD_SendCommand(0x0C);  // Display ON
    LCD_SendCommand(0x06);  // Auto increment
    LCD_I2C_Clear();
}

void LCD_I2C_Clear(void)
{
    LCD_SendCommand(0x01);
    for (volatile int i=0;i<50000;i++);
}

void LCD_I2C_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t command = (row == 0 ? 0x80 : 0xC0) + col;
    LCD_SendCommand(command);
}

void LCD_I2C_WriteChar(char c)
{
    LCD_SendData(c);
}

void LCD_I2C_WriteString(const char *str)
{
    while (*str)
        LCD_SendData(*str++);
}

// Clears a single line (row: 0 or 1)
void LCD_I2C_ClearLine(uint8_t row)
{
    // Move cursor to the beginning of the specified row
    LCD_I2C_SetCursor(row, 0);

    // Write spaces across the whole line (assume 16 characters)
    for (uint8_t i = 0; i < 16; i++)
    {
        LCD_I2C_WriteChar(' ');
    }

    // Return cursor to beginning of the line
    LCD_I2C_SetCursor(row, 0);
}
