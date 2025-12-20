/******************************************************************************
 * File: hmi.c
 * Module: HMI (Human-Machine Interface) Application Layer
 * Description: Door Locker Security System - HMI ECU Implementation
 * Author: Improved Implementation
 * Date: December 18, 2025
 ******************************************************************************/

#include "hmi.h"
#include "../HAL/lcd/lcd.h"
#include "../HAL/keypad/keypad.h"
#include "../HAL/led/led.h"
#include "../HAL/pot/pot.h"
#include "../MCAL/timers/systick.h"
#include "../../Common/HAL/comm_interface.h"
#include <string.h>
#include <stdio.h>

/******************************************************************************
 *                       Static Function Prototypes                            *
 ******************************************************************************/

static void HMI_Delay_Seconds(uint16_t seconds);
static uint8_t HMI_WaitForKey(void);
static void HMI_ShowCountdown(const char* message, uint16_t seconds);

/******************************************************************************
 *                       Function Implementations                              *
 ******************************************************************************/

void HMI_Init(void)
{
    /* Initialize all hardware peripherals */
    LCD_I2C_Init();
    Keypad_Init();
    LED_init();
    POT_Init();
    COMM_Init();

    /* Initialize SysTick for 1ms delays */
    SysTick_Init(16000, SYSTICK_NOINT);

    /* Display welcome message */
    HMI_DisplayMessage("Door Locker", "System v1.0");
    LED_setOn(LED_BLUE);
    HMI_Delay_Seconds(2);
}

void DisplayConnection() {
    HMI_DisplayMessage("Connected!", "");
    LED_setOn(LED_GREEN);
    HMI_Delay_Seconds(1);
}

void HMI_Task(void)
{
    LED_setOn(LED_GREEN);
    HMI_ShowMainMenu();

    /* Wait for user input */
    char key = HMI_WaitForKey();

    /* Process menu selection */
    switch(key)
    {
        case KEY_OPEN_DOOR:
            HMI_HandleOpenDoor();
            break;

        case KEY_CHANGE_PASSWORD:
            HMI_HandleChangePassword();
            break;

        case KEY_SET_TIMEOUT:
            HMI_HandleSetTimeout();
            break;

        default:
            /* Invalid key, stay in menu */
            break;
    }
}

void HMI_GetPasswordInput(char* buffer)
{
    uint8_t idx = 0;

    /* Clear buffer */
    memset(buffer, 0, PASSWORD_LENGTH + 1);

    /* Move cursor to second line for password entry */
    LCD_I2C_SetCursor(1, 0);

    /* Clear any pending inputs */
    HMI_ClearKeypadBuffer();

    /* Get PASSWORD_LENGTH digits */
    while(idx < PASSWORD_LENGTH)
    {
        char key = 0;

        /* Wait for valid numeric key (0-9) */
        while(1)
        {
            key = Keypad_GetKey();
            if(key >= '0' && key <= '9')
            {
                break;
            }
            DelayMs(50);
        }

        /* Store the key and display '*' */
        buffer[idx] = key;
        LCD_I2C_WriteChar('*');
        idx++;

        /* Debounce delay */
        DelayMs(300);

        /* Clear keypad buffer to avoid double-press */
        HMI_ClearKeypadBuffer();
    }

    /* Null terminate */
    buffer[PASSWORD_LENGTH] = '\0';
}

uint8_t HMI_VerifyPassword(const char* password)
{
    /* Send command to verify password */
    COMM_SendCommand(CMD_SEND_PASSWORD);

    /* Send password */
    COMM_SendMessage((const uint8_t*)password);

    for (;;) {
        /* Wait for response */
        uint8_t response = COMM_ReceiveCommand();

        if(response == CMD_PASSWORD_CORRECT)
        {
            COMM_SendCommand(CMD_ACK);
            return 1;  /* Password correct */
        }
        else if (response == CMD_PASSWORD_WRONG) {
            COMM_SendCommand(CMD_ACK);
            return 0;  /* Password incorrect */
        }
    }
}

void HMI_SavePassword(const char* password)
{
    /* Send command to change/save password */
    COMM_SendCommand(CMD_CHANGE_PASSWORD);

    /* Send password string */
    COMM_SendMessage((const uint8_t*)password);

    /* Wait for acknowledgment */
    while (COMM_ReceiveCommand() != CMD_ACK) { }
}

uint8_t HMI_SetupPassword(void)
{
    char password1[PASSWORD_LENGTH + 1];
    char password2[PASSWORD_LENGTH + 1];

    /* Step 1: Enter new password */
    HMI_DisplayMessage("Enter New", "Password:");
    HMI_Delay_Seconds(1);
    HMI_GetPasswordInput(password1);
    DelayMs(500);

    /* Step 2: Re-enter password for confirmation */
    HMI_DisplayMessage("Re-enter", "Password:");
    HMI_Delay_Seconds(1);
    HMI_GetPasswordInput(password2);
    DelayMs(500);

    /* Step 3: Check if passwords match */
    if(strncmp(password1, password2, PASSWORD_LENGTH) == 0)
    {
        /* Passwords match - send to Control ECU for storage */
        HMI_SavePassword(password1);
        return 1;
    }
    else
    {
        /* Passwords don't match */
        HMI_DisplayMessage("Passwords", "Don't Match!");
        LED_setOn(LED_RED);
        HMI_Delay_Seconds(2);
    }

    return 0;  /* Failed */
}

void HMI_ShowMainMenu(void)
{

    LCD_I2C_Clear();
    LCD_I2C_SetCursor(0, 0);
    LCD_I2C_WriteString("A:Open  B:Chg");
    LCD_I2C_SetCursor(1, 0);
    LCD_I2C_WriteString("C:Time  D:Canc");

}

uint8_t HMI_HandleOpenDoor(void)
{
    char password[PASSWORD_LENGTH + 1];

    /* Request password from user */
    HMI_DisplayMessage("Enter Password", "to Open Door:");
    LED_setOn(LED_BLUE);
    HMI_GetPasswordInput(password);

    /* Verify password with Control ECU */
    if(HMI_VerifyPassword(password))
    {
        DelayMs(50);
        /* Password correct - send door unlock command */
        COMM_SendCommand(CMD_DOOR_UNLOCK);

        while (COMM_ReceiveCommand() != CMD_ACK) { }

        /* Display unlocking status */
        LED_setOn(LED_GREEN);
        HMI_DisplayMessage("Unlocked Door", "");
        DelayMs(2000);
        
        return 1;  /* Success */
    }
    else
    {
        COMM_SendCommand(CMD_ACK);
        LED_setOn(LED_RED);

        HMI_DisplayMessage("Incorrect Password!", "");
        DelayMs(2000);

        return 0;
    }
}

uint8_t HMI_HandleChangePassword(void)
{
    char oldPassword[PASSWORD_LENGTH + 1];

    /* Request old password */
    HMI_DisplayMessage("Enter Old", "Password:");
    LED_setOn(LED_BLUE);
    HMI_GetPasswordInput(oldPassword);
    DelayMs(500);

    /* Verify old password */
    if(HMI_VerifyPassword(oldPassword))
    {
        /* Old password correct - setup new password */
        return HMI_SetupPassword();
    }
    else
    {
        LED_setOn(LED_RED);
        HMI_DisplayMessage("Wrong Password!", "");
        HMI_Delay_Seconds(2);
        return 0;
    }
}

uint8_t HMI_HandleSetTimeout(void)
{
    uint32_t timeout;
    char buffer[17];

    HMI_DisplayMessage("Adjust Timeout", "(# Save, C Exit)");
    LED_setOn(LED_BLUE);
    HMI_Delay_Seconds(2);

    /* Allow user to adjust timeout with potentiometer */
    while(1)
    {
        /* Read potentiometer and map to timeout range */
        timeout = POT_ReadMapped(MIN_TIMEOUT_SEC, MAX_TIMEOUT_SEC);

        /* Display current value */
        LCD_I2C_Clear();
        LCD_I2C_SetCursor(0, 0);
        LCD_I2C_WriteString("Timeout:");
        LCD_I2C_SetCursor(1, 0);
        snprintf(buffer, sizeof(buffer), "%u seconds", timeout);
        LCD_I2C_WriteString(buffer);

        /* Check for user action */
        char key = Keypad_GetKey();

        if(key == KEY_CONFIRM)  /* # to save */
        {
            /* Request password confirmation */
            char password[PASSWORD_LENGTH + 1];
            HMI_DisplayMessage("Enter Password", "to Confirm:");
            HMI_Delay_Seconds(1);
            HMI_GetPasswordInput(password);
            DelayMs(500);

            if(HMI_VerifyPassword(password))
            {
                /* Password correct - save timeout */
                uint8_t newTimeout = timeout;
                COMM_SendCommand(CMD_SET_TIMEOUT);
                COMM_SendMessage((const uint8_t*)&newTimeout);

                for(;;) {
                    if (COMM_ReceiveCommand() == CMD_SUCCESS) {
                        HMI_DisplayMessage("Timeout Saved!", "");
                        LED_setOn(LED_GREEN);
                        return 1;
                    } else if (COMM_ReceiveCommand() == CMD_FAIL) {
                        HMI_DisplayMessage("Error saving", "");
                        LED_setOn(LED_RED);
                        return 0;
                    }
                }
            }
            else
            {
                /* Wrong password */
                HMI_DisplayMessage("Wrong Password", "Not Saved");
                LED_setOn(LED_RED);
                HMI_Delay_Seconds(2);
                return 0;
            }
        }
        else if(key == KEY_CANCEL)  /* C to cancel */
        {
            HMI_DisplayMessage("Cancelled", "");
            HMI_Delay_Seconds(1);
            return 0;
        }

        DelayMs(200);  /* Update rate */
    }
}

void HMI_DisplayDoorStatus(const char* status, uint16_t seconds)
{
    char buffer[17];

    LCD_I2C_Clear();
    LCD_I2C_SetCursor(0, 0);
    LCD_I2C_WriteString(status);

    if(seconds > 0)
    {
        LCD_I2C_SetCursor(1, 0);
        snprintf(buffer, sizeof(buffer), "%u seconds", seconds);
        LCD_I2C_WriteString(buffer);
    }
}

void HMI_HandleLockout(void)
{
    /* Send alarm command to Control ECU */
    COMM_SendCommand(CMD_ALARM);

    LED_setOn(LED_RED);

    /* Display lockout message with countdown */
    HMI_ShowCountdown("LOCKED OUT!", LOCKOUT_DURATION_SEC);

    HMI_DisplayMessage("Lockout Ended", "");
    LED_setOn(LED_GREEN);
    HMI_Delay_Seconds(2);
}

void HMI_DisplayMessage(const char* line1, const char* line2)
{
    LCD_I2C_Clear();
    LCD_I2C_SetCursor(0, 0);
    LCD_I2C_WriteString(line1);

    if(line2 != NULL && strlen(line2) > 0)
    {
        LCD_I2C_SetCursor(1, 0);
        LCD_I2C_WriteString(line2);
    }
}

void HMI_ClearKeypadBuffer(void)
{
    /* Read and discard any pending keypad inputs */
    while(Keypad_GetKey() != 0)
    {
        DelayMs(10);
    }
}

/******************************************************************************
 *                       Static Helper Functions                               *
 ******************************************************************************/

static void HMI_Delay_Seconds(uint16_t seconds)
{
    for(uint16_t i = 0; i < seconds; i++)
    {
        DelayMs(1000);
    }
}

static uint8_t HMI_WaitForKey(void)
{
    char key = '/';

    /* Wait for valid key press */
    while(key == '/')
    {
        key = Keypad_GetKey();
        DelayMs(50);
    }

    /* Debounce */
    DelayMs(200);
    HMI_ClearKeypadBuffer();

    return key;
}

static void HMI_ShowCountdown(const char* message, uint16_t seconds)
{
    char buffer[17];

    for(uint16_t i = seconds; i > 0; i--)
    {
        LCD_I2C_Clear();
        LCD_I2C_SetCursor(0, 0);
        LCD_I2C_WriteString(message);
        LCD_I2C_SetCursor(1, 0);
        snprintf(buffer, sizeof(buffer), "%u seconds", i);
        LCD_I2C_WriteString(buffer);

        DelayMs(1000);
    }
}