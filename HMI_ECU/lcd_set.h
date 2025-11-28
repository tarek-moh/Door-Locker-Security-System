#ifndef LCD_H
#define LCD_H
// ==============================
// LCD 16x2 CONNECTIONS TO Tiva C
// ==============================
// RS  -> PB0   // Register Select (0=Command, 1=Data)
// EN  -> PB1   // Enable signal
// D4  -> PB4   // Data bit 4
// D5  -> PB5   // Data bit 5
// D6  -> PB6   // Data bit 6
// D7  -> PB7   // Data bit 7
// VSS -> GND
// VDD -> +5V
// V0  -> Potentiometer middle pin (contrast), other ends to GND & +5V
// RW  -> GND   // Always write
// A   -> +5V   // Backlight Anode
// K   -> GND   // Backlight Cathode
// ==============================
void LCD_init();
void LCD_command(uint8_t cmd);
void LCD_data(uint8_t data);
void LCD_print(char *str);

#endif
