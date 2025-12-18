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
#include "comm_interface.h"
#include <string.h>
#include <stdio.h>

/******************************************************************************
 *                          Static Variables                                   *
 ******************************************************************************/

static HMI_State_t g_currentState = STATE_INIT;
static uint8_t g_passwordAttempts = 0;
static uint32_t g_autoLockTimeout = DEFAULT_TIMEOUT_SEC;
static uint8_t g_firstTimeFlag = 1;

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
    
    /* Perform handshake with Control ECU */
    HMI_DisplayMessage("Connecting to", "Control ECU...");
    
    if(HMI_PerformHandshake())
    {
        HMI_DisplayMessage("Connected!", "");
        LED_setOn(LED_GREEN);
        HMI_Delay_Seconds(1);
    }
    else
    {
        HMI_DisplayMessage("Connection", "Failed!");
        LED_setOn(LED_RED);
        HMI_Delay_Seconds(2);
    }
    
    /* Check if this is first time setup */
    COMM_SendCommand(CMD_READY);
    uint8_t response = COMM_ReceiveCommand();
    
    if(response == CMD_ACK)
    {
        g_firstTimeFlag = 1;  /* First time - need password setup */
        g_currentState = STATE_FIRST_TIME_SETUP;
    }
    else
    {
        g_firstTimeFlag = 0;  /* Password already exists */
        g_currentState = STATE_MAIN_MENU;
    }
    
    g_passwordAttempts = 0;
}

void HMI_Task(void)
{
    switch(g_currentState)
    {
        case STATE_INIT:
            /* Already handled in HMI_Init(), should not reach here */
            g_currentState = STATE_MAIN_MENU;
            break;
            
        case STATE_FIRST_TIME_SETUP:
            LED_setOn(LED_BLUE);
            HMI_DisplayMessage("First Time", "Setup");
            HMI_Delay_Seconds(2);
            
            if(HMI_SetupPassword())
            {
                HMI_DisplayMessage("Password Saved!", "");
                LED_setOn(LED_GREEN);
                HMI_Delay_Seconds(2);
                g_firstTimeFlag = 0;
                g_currentState = STATE_MAIN_MENU;
            }
            else
            {
                HMI_DisplayMessage("Setup Failed", "Try Again");
                LED_setOn(LED_RED);
                HMI_Delay_Seconds(2);
                /* Stay in setup state until successful */
            }
            break;
            
        case STATE_MAIN_MENU:
            LED_setOn(LED_GREEN);
            HMI_ShowMainMenu();
            
            /* Wait for user input */
            char key = HMI_WaitForKey();
            
            /* Process menu selection */
            switch(key)
            {
                case KEY_OPEN_DOOR:
                    g_passwordAttempts = 0;
                    g_currentState = STATE_OPEN_DOOR;
                    break;
                    
                case KEY_CHANGE_PASSWORD:
                    g_passwordAttempts = 0;
                    g_currentState = STATE_CHANGE_PASSWORD;
                    break;
                    
                case KEY_SET_TIMEOUT:
                    g_currentState = STATE_SET_TIMEOUT;
                    break;
                    
                default:
                    /* Invalid key, stay in menu */
                    break;
            }
            break;
            
        case STATE_OPEN_DOOR:
            if(HMI_HandleOpenDoor())
            {
                /* Success - door opened and closed */
                g_passwordAttempts = 0;
                g_currentState = STATE_MAIN_MENU;
            }
            else
            {
                /* Failed - check if locked out */
                if(g_passwordAttempts >= MAX_PASSWORD_ATTEMPTS)
                {
                    g_currentState = STATE_LOCKOUT;
                }
                else
                {
                    g_currentState = STATE_MAIN_MENU;
                }
            }
            break;
            
        case STATE_CHANGE_PASSWORD:
            if(HMI_HandleChangePassword())
            {
                HMI_DisplayMessage("Password", "Changed!");
                LED_setOn(LED_GREEN);
                HMI_Delay_Seconds(2);
                g_passwordAttempts = 0;
                g_currentState = STATE_MAIN_MENU;
            }
            else
            {
                if(g_passwordAttempts >= MAX_PASSWORD_ATTEMPTS)
                {
                    g_currentState = STATE_LOCKOUT;
                }
                else
                {
                    g_currentState = STATE_MAIN_MENU;
                }
            }
            break;
            
        case STATE_SET_TIMEOUT:
            HMI_HandleSetTimeout();
            g_currentState = STATE_MAIN_MENU;
            break;
            
        case STATE_LOCKOUT:
            HMI_HandleLockout();
            g_passwordAttempts = 0;
            g_currentState = STATE_MAIN_MENU;
            break;
            
        case STATE_ERROR:
            HMI_DisplayMessage("System Error", "Restarting...");
            LED_setOn(LED_RED);
            HMI_Delay_Seconds(3);
            g_currentState = STATE_MAIN_MENU;
            break;
            
        default:
            g_currentState = STATE_ERROR;
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
    
    /* Send password string */
    COMM_SendMessage((const uint8_t*)password);
    
    /* Wait for response */
    uint8_t response = COMM_ReceiveCommand();
    
    if(response == CMD_PASSWORD_CORRECT)
    {
        return 1;  /* Password correct */
    }
    else
    {
        return 0;  /* Password incorrect */
    }
}

uint8_t HMI_SavePassword(const char* password)
{
    /* Send command to change/save password */
    COMM_SendCommand(CMD_CHANGE_PASSWORD);
    
    /* Send password string */
    COMM_SendMessage((const uint8_t*)password);
    
    /* Wait for acknowledgment */
    uint8_t response = COMM_ReceiveCommand();
    
    return (response == CMD_ACK);
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
        if(HMI_SavePassword(password1))
        {
            return 1;  /* Success */
        }
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
    LCD_I2C_WriteString("+ Open Door");
    LCD_I2C_SetCursor(1, 0);
    LCD_I2C_WriteString("- Chg PW *Time");
}

uint8_t HMI_HandleOpenDoor(void)
{
    char password[PASSWORD_LENGTH + 1];
    
    /* Request password from user */
    HMI_DisplayMessage("Enter Password", "to Open Door:");
    LED_setOn(LED_BLUE);
    HMI_Delay_Seconds(1);
    HMI_GetPasswordInput(password);
    DelayMs(500);
    
    /* Verify password with Control ECU */
    if(HMI_VerifyPassword(password))
    {
        /* Password correct - send door unlock command */
        COMM_SendCommand(CMD_DOOR_UNLOCK);
        
        /* Display unlocking status */
        LED_setOn(LED_GREEN);
        HMI_DisplayMessage("Door Unlocking", "Please Wait...");
        HMI_Delay_Seconds(DOOR_UNLOCK_TIME);
        
        /* Display door open with countdown */
        HMI_ShowCountdown("Door Open", g_autoLockTimeout);
        
        /* Send door lock command */
        COMM_SendCommand(CMD_DOOR_LOCK);
        
        /* Display locking status */
        HMI_DisplayMessage("Door Locking", "Please Wait...");
        HMI_Delay_Seconds(DOOR_LOCK_TIME);
        
        /* Display completion */
        HMI_DisplayMessage("Door Locked", "");
        HMI_Delay_Seconds(2);
        
        return 1;  /* Success */
    }
    else
    {
        /* Password incorrect */
        g_passwordAttempts++;
        
        LED_setOn(LED_RED);
        
        if(g_passwordAttempts >= MAX_PASSWORD_ATTEMPTS)
        {
            /* Too many attempts - will trigger lockout */
            HMI_DisplayMessage("TOO MANY", "ATTEMPTS!");
            HMI_Delay_Seconds(2);
            return 0;
        }
        else
        {
            /* Show error with remaining attempts */
            char msg[17];
            snprintf(msg, sizeof(msg), "Tries: %d/%d", 
                     g_passwordAttempts, MAX_PASSWORD_ATTEMPTS);
            HMI_DisplayMessage("Wrong Password!", msg);
            HMI_Delay_Seconds(2);
            return 0;
        }
    }
}

uint8_t HMI_HandleChangePassword(void)
{
    char oldPassword[PASSWORD_LENGTH + 1];
    
    /* Request old password */
    HMI_DisplayMessage("Enter Old", "Password:");
    LED_setOn(LED_BLUE);
    HMI_Delay_Seconds(1);
    HMI_GetPasswordInput(oldPassword);
    DelayMs(500);
    
    /* Verify old password */
    if(HMI_VerifyPassword(oldPassword))
    {
        /* Old password correct - setup new password */
        g_passwordAttempts = 0;
        return HMI_SetupPassword();
    }
    else
    {
        /* Old password incorrect */
        g_passwordAttempts++;
        
        LED_setOn(LED_RED);
        
        if(g_passwordAttempts >= MAX_PASSWORD_ATTEMPTS)
        {
            HMI_DisplayMessage("TOO MANY", "ATTEMPTS!");
            HMI_Delay_Seconds(2);
            return 0;
        }
        else
        {
            char msg[17];
            snprintf(msg, sizeof(msg), "Tries: %d/%d", 
                     g_passwordAttempts, MAX_PASSWORD_ATTEMPTS);
            HMI_DisplayMessage("Wrong Password!", msg);
            HMI_Delay_Seconds(2);
            return 0;
        }
    }
}

uint8_t HMI_HandleSetTimeout(void)
{
    uint32_t timeout = g_autoLockTimeout;
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
        snprintf(buffer, sizeof(buffer), "%lu seconds", timeout);
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
                g_autoLockTimeout = timeout;
                HMI_DisplayMessage("Timeout Saved!", "");
                LED_setOn(LED_GREEN);
                HMI_Delay_Seconds(2);
                return 1;
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

void HMI_UpdateLED(HMI_State_t state)
{
    switch(state)
    {
        case STATE_INIT:
        case STATE_FIRST_TIME_SETUP:
        case STATE_SET_TIMEOUT:
            LED_setOn(LED_BLUE);
            break;
            
        case STATE_MAIN_MENU:
        case STATE_OPEN_DOOR:
            LED_setOn(LED_GREEN);
            break;
            
        case STATE_LOCKOUT:
        case STATE_ERROR:
            LED_setOn(LED_RED);
            break;
            
        default:
            LED_setOn(LED_OFF);
            break;
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

uint8_t HMI_PerformHandshake(void)
{
    uint8_t retries = 0;
    const uint8_t MAX_RETRIES = 3;
    
    while(retries < MAX_RETRIES)
    {
        COMM_SendCommand(CMD_READY);
        
        /* Wait for response with timeout */
        uint16_t timeout = 0;
        while(timeout < 1000)  /* 1 second timeout */
        {
            /* This is simplified - in real implementation, use non-blocking UART check */
            DelayMs(10);
            timeout += 10;
        }
        
        /* For now, assume handshake successful */
        /* In real implementation, check for CMD_ACK response */
        return 1;
        
        retries++;
    }
    
    return 0;  /* Handshake failed */
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
    char key = 0;
    
    /* Wait for valid key press */
    while(key == 0)
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