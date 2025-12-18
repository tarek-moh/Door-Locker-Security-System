#include "tm4c123gh6pm.h"
#include "gpio.h"
#include "keypad.h"
#include "lcd.h"
#include "pot.h"
#include "systick.h"
#include <stdint.h>
#include <string.h>

#define PASSWORD_LENGTH 5
#define MAX_TIMEOUT 30
#define MIN_TIMEOUT 5

char password[PASSWORD_LENGTH + 1] = "12345"; // Default password
uint32_t autolock_timeout = 10;               // Default timeout in seconds

// Function prototypes
void HMI_Init(void);
void GetPassword(char* input);
uint8_t VerifyPassword(const char* input);
void ShowMainMenu(void);
void SetAutoLockTimeout(void);
void ChangePassword(void);

int main(void)
{
    char input[PASSWORD_LENGTH + 1];

    HMI_Init();

    LCD_I2C_Clear();
    LCD_I2C_WriteString("Enter Password:");

    while(1)
    {
        GetPassword(input);
        if (VerifyPassword(input))
        {
            while(1)
            {
                ShowMainMenu();
                char key = Keypad_GetKey();
                if (key == '+')   // Open Door simulation
                {
                    LCD_I2C_Clear();
                    LCD_I2C_WriteString("Door Opened!");
                    DelayMs(autolock_timeout * 1000);
                    LCD_I2C_Clear();
                    LCD_I2C_WriteString("Door Locked!");
                    DelayMs(2000);
                }
                else if (key == '-') // Change Password
                {
                    ChangePassword();
                }
                else if (key == '*') // Set Auto-Lock Timeout
                {
                    SetAutoLockTimeout();
                }
            }
        }
        else
        {
            LCD_I2C_Clear();
            LCD_I2C_WriteString("Wrong Password!");
            DelayMs(2000);
            LCD_I2C_Clear();
            LCD_I2C_WriteString("Enter Password:");
        }
    }
}

void HMI_Init(void)
{
    Keypad_Init();
    LCD_I2C_Init();
    POT_Init();
    SysTick_Init(16000, SYSTICK_NOINT); // 1ms tick for delay
}

// Reads password from keypad, displays '*' for each digit
void GetPassword(char* input)
{
    uint8_t idx = 0;
    LCD_I2C_Clear();
    LCD_I2C_WriteString("Enter Password:");
    LCD_I2C_SetCursor(1,0);
    memset(input, 0, PASSWORD_LENGTH + 1);

    while (idx < PASSWORD_LENGTH)
    {
        char key = 0;
        while ((key = Keypad_GetKey()) == 0);
        input[idx++] = key;
        LCD_I2C_WriteChar('*');
        DelayMs(300); // simple debounce
    }
}

// Verify entered password
uint8_t VerifyPassword(const char* input)
{
    return (strncmp(password, input, PASSWORD_LENGTH) == 0);
}

// Display main menu
void ShowMainMenu(void)
{
    LCD_I2C_Clear();
    LCD_I2C_WriteString("+ Open Door");
    LCD_I2C_SetCursor(1,0);
    LCD_I2C_WriteString("- Change PW  * Timeout");
}

// Change password flow
void ChangePassword(void)
{
    char old_pw[PASSWORD_LENGTH + 1];
    char new_pw[PASSWORD_LENGTH + 1];

    LCD_I2C_Clear();
    LCD_I2C_WriteString("Enter Old PW:");
    GetPassword(old_pw);

    if (VerifyPassword(old_pw))
    {
        LCD_I2C_Clear();
        LCD_I2C_WriteString("New Password:");
        GetPassword(new_pw);
        memcpy(password, new_pw, PASSWORD_LENGTH);
        LCD_I2C_Clear();
        LCD_I2C_WriteString("Password Saved!");
        DelayMs(2000);
    }
    else
    {
        LCD_I2C_Clear();
        LCD_I2C_WriteString("Wrong Old PW!");
        DelayMs(2000);
    }
}

// Set auto-lock timeout via potentiometer
void SetAutoLockTimeout(void)
{
    LCD_I2C_Clear();
    LCD_I2C_WriteString("Set Timeout:");

    uint32_t timeout = 0;
    while (1)
    {
        timeout = POT_ReadMapped(MIN_TIMEOUT, MAX_TIMEOUT);
        LCD_I2C_SetCursor(1,0);
        char buf[16];
        snprintf(buf, sizeof(buf), "%2lu sec   ", timeout);
        LCD_I2C_WriteString(buf);

        char key = Keypad_GetKey();
        if (key == '#') // Save
        {
            autolock_timeout = timeout;
            LCD_I2C_Clear();
            LCD_I2C_WriteString("Timeout Saved!");
            DelayMs(2000);
            break;
        }
    }
}