/* main.c - HMI Application (uses your systick.c API: SysTick_Init / DelayMs) */

#include <string.h>
#include <stdint.h>

#include "comm_interface.h"
#include "lcd.h"
#include "keypad.h"
#include "systick.h"
#include "pot.h"

#define PASS_LENGTH 5
#define MAX_ATTEMPTS 3

/* Prototypes */
void InitialPasswordSetup(void);
void OpenDoorFlow(void);
uint8_t EnterPassword(uint8_t *pass);
uint8_t CheckPasswordWithControl(uint8_t *pass);
void ChangePasswordFlow(void);
void SetAutoLockTimeout(void);

/* Main */
int main(void)
{
    /* Initialize peripherals (names kept as in your original files) */
    LCD_init();
    KEYPAD_init();
    COMM_Init();
    POT_init(); /* if your pot init name differs, change back to your actual name */

    /* Initialize SysTick for 1 ms tick using interrupts */
    SysTick_Init(16000, SYSTICK_INT); /* 16 MHz clock -> reload = 16000 -> 1 ms tick */

    /* Handshake with Control ECU */
    COMM_SendCommand(CMD_READY);
    if (COMM_ReceiveCommand() != CMD_ACK)
    {
        LCD_clearScreen();
        LCD_displayString("Control ECU Err");
        while (1); /* hang */
    }

    /* Initial password setup (if needed) */
    InitialPasswordSetup();

    /* Main menu loop */
    while (1)
    {
        LCD_clearScreen();
        LCD_displayString("+ Open Door");
        LCD_moveCursor(1,0);
        LCD_displayString("- Change Pass");
        LCD_moveCursor(2,0);
        LCD_displayString("* Timeout");

        uint8_t key = KEYPAD_getPressedKey();

        if (key == '+')
        {
            OpenDoorFlow();
        }
        else if (key == '-')
        {
            ChangePasswordFlow();
        }
        else if (key == '*')
        {
            SetAutoLockTimeout();
        }
        /* small debounce / CPU relief */
        DelayMs(100);
    }
}

/* Initial Password Setup */
void InitialPasswordSetup(void)
{
    uint8_t pass1[PASS_LENGTH + 1];
    uint8_t pass2[PASS_LENGTH + 1];

    while (1)
    {
        LCD_clearScreen();
        LCD_displayString("Enter Password:");
        EnterPassword(pass1);

        LCD_clearScreen();
        LCD_displayString("Confirm:");
        EnterPassword(pass2);

        if (strcmp((char*)pass1, (char*)pass2) == 0)
        {
            COMM_SendCommand(CMD_CHANGE_PASSWORD);
            COMM_SendMessage(pass1); /* Make sure COMM_SendMessage expects null-terminated */

            if (COMM_ReceiveCommand() == CMD_ACK)
            {
                LCD_clearScreen();
                LCD_displayString("Saved!");
                DelayMs(1000);
                break;
            }
            else
            {
                LCD_clearScreen();
                LCD_displayString("Comm Err");
                DelayMs(1000);
            }
        }
        else
        {
            LCD_clearScreen();
            LCD_displayString("Not Match!");
            DelayMs(1000);
        }
    }
}

/* Read password from keypad (shows '*' on LCD) */
uint8_t EnterPassword(uint8_t *pass)
{
    for (uint8_t i = 0; i < PASS_LENGTH; i++)
    {
        uint8_t key = KEYPAD_getPressedKey();
        /* Keep waiting if no key (adjust if your keypad returns 0 for none) */
        while (key == 0)
        {
            DelayMs(20);
            key = KEYPAD_getPressedKey();
        }
        pass[i] = key;
        LCD_displayCharacter('*');
        /* small debounce */
        DelayMs(200);
    }
    pass[PASS_LENGTH] = '\0';
    return 1;
}

/* Open Door Flow */
void OpenDoorFlow(void)
{
    uint8_t attempts = 0;
    uint8_t pass[PASS_LENGTH + 1];

    while (attempts < MAX_ATTEMPTS)
    {
        LCD_clearScreen();
        LCD_displayString("Enter Pass:");
        EnterPassword(pass);

        if (CheckPasswordWithControl(pass))
        {
            /* Correct password -> request unlock */
            COMM_SendCommand(CMD_DOOR_UNLOCK);
            /* wait for ACK */
            if (COMM_ReceiveCommand() == CMD_ACK)
            {
                LCD_clearScreen();
                LCD_displayString("Unlocking...");
                /* keep door open for configured timeout (Control ECU handles actual timing usually) */
                DelayMs(3000); /* local UI delay while Control ECU handles motor */
            }

            /* send lock command */
            COMM_SendCommand(CMD_DOOR_LOCK);
            /* wait for ACK */
            (void)COMM_ReceiveCommand();

            LCD_clearScreen();
            LCD_displayString("Done");
            DelayMs(1000);
            return;
        }
        else
        {
            attempts++;
            LCD_clearScreen();
            LCD_displayString("Wrong!");
            DelayMs(800);
        }
    }

    /* 3 failed attempts -> alarm */
    COMM_SendCommand(CMD_ALARM);
    LCD_clearScreen();
    LCD_displayString("LOCKED!");
    DelayMs(5000);
}

/* Send password to Control ECU and check response */
uint8_t CheckPasswordWithControl(uint8_t *pass)
{
    COMM_SendCommand(CMD_SEND_PASSWORD);
    COMM_SendMessage(pass);
    uint8_t response = COMM_ReceiveCommand();
    return (response == CMD_PASSWORD_CORRECT) ? 1 : 0;
}

/* Change password flow */
void ChangePasswordFlow(void)
{
    uint8_t attempts = 0;
    uint8_t oldPass[PASS_LENGTH + 1];

    while (attempts < MAX_ATTEMPTS)
    {
        LCD_clearScreen();
        LCD_displayString("Old Password:");
        EnterPassword(oldPass);

        if (CheckPasswordWithControl(oldPass))
        {
            InitialPasswordSetup();
            return;
        }
        else
        {
            attempts++;
            LCD_clearScreen();
            LCD_displayString("Wrong!");
            DelayMs(800);
        }
    }

    /* Lockout and alarm */
    COMM_SendCommand(CMD_ALARM);
    LCD_clearScreen();
    LCD_displayString("LOCKED!");
    DelayMs(5000);
}

/* Set Auto-Lock Timeout (reads potentiometer) */
void SetAutoLockTimeout(void)
{
    uint8_t pass[PASS_LENGTH + 1];
    uint8_t timeout = 5;

    while (1)
    {
        LCD_clearScreen();
        LCD_displayString("Timeout: ");
        /* use your POT reading API; keep original name POT_getValue */
        timeout = POT_getValue(5, 30); /* make sure POT_getValue exists and returns 5..30 */
        LCD_intgerToString(timeout);    /* spelled as in your original code */
        LCD_displayString(" sec");

        LCD_moveCursor(2,0);
        LCD_displayString("Press = to save");

        /* wait key press - small debounce */
        uint8_t k = KEYPAD_getPressedKey();
        if (k == '=')
            break;
        DelayMs(200);
    }

    /* confirm by password */
    LCD_clearScreen();
    LCD_displayString("Enter Pass:");
    EnterPassword(pass);

    if (CheckPasswordWithControl(pass))
    {
        COMM_SendCommand(CMD_SEND_TIMEOUT);
        COMM_SendByte(timeout); /* ensure COMM_SendByte exists */
        if (COMM_ReceiveCommand() == CMD_ACK)
        {
            LCD_clearScreen();
            LCD_displayString("Saved!");
            DelayMs(1000);
        }
    }
    else
    {
        LCD_clearScreen();
        LCD_displayString("Wrong!");
        DelayMs(800);
    }
}
