#include "comm_interface.h"
#include "lcd.h"
#include "keypad.h"
#include "timers.h" // For TIMER_init and TIMER_start (still not implemented )
#include "potentiometer.h"// For POT_init and POT_getValue( still not implemented )

#define PASS_LENGTH 5
#define MAX_ATTEMPTS 3


/************************* Function Prototypes *************************/
void InitialPasswordSetup(void);
void OpenDoorFlow(void);
uint8_t EnterPassword(uint8_t *pass);
uint8_t CheckPasswordWithControl(uint8_t *pass);
void ChangePasswordFlow(void);
void SetAutoLockTimeout(void);


/******************************* MAIN *********************************/
int main(void)
{
    LCD_init();
    KEYPAD_init();
    COMM_Init();
    TIMER_init();
    POT_init();

    /* Handshake */
    COMM_SendCommand(CMD_READY);
    if (COMM_ReceiveCommand() != CMD_ACK)
    {
        LCD_displayString("Control ECU Err");
        while(1);
    }

    /* Step 1 — First-time password setup */
    InitialPasswordSetup();

    /* Step 2 — Main Menu Loop */
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
            OpenDoorFlow();
        else if (key == '-')
            ChangePasswordFlow();
        else if (key == '*')
            SetAutoLockTimeout();
    }
}


/************************* Step 1: Initial Password Setup *************************/
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
            COMM_SendMessage(pass1);

            if (COMM_ReceiveCommand() == CMD_ACK)
            {
                LCD_clearScreen();
                LCD_displayString("Saved!");
                _delay_ms(1000);
                break;
            }
        }
        else
        {
            LCD_clearScreen();
            LCD_displayString("Not Match!");
            _delay_ms(1000);
        }
    }
}


/************************* Helper: Read password from keypad *************************/
uint8_t EnterPassword(uint8_t *pass)
{
    for (uint8_t i = 0; i < PASS_LENGTH; i++)
    {
        uint8_t key = KEYPAD_getPressedKey();
        pass[i] = key;
        LCD_displayCharacter('*');
    }
    pass[PASS_LENGTH] = '\0';
    return 1;
}


/************************* Step 3: Open Door *************************/
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
            /* Correct password */
            COMM_SendCommand(CMD_DOOR_UNLOCK);
            COMM_ReceiveCommand(); // Expect ACK

            LCD_clearScreen();
            LCD_displayString("Unlocking...");
            TIMER_start(3); 

            /* after timeout door locks again */
            LCD_clearScreen();
            LCD_displayString("Locking...");
            COMM_SendCommand(CMD_DOOR_LOCK);
            COMM_ReceiveCommand(); // Expect ACK

            _delay_ms(1000);
            return;
        }
        else
        {
            attempts++;
            LCD_clearScreen();
            LCD_displayString("Wrong!");
            _delay_ms(800);
        }
    }

    /* If 3 failed attempts → alarm */
    COMM_SendCommand(CMD_ALARM);
    LCD_clearScreen();
    LCD_displayString("LOCKED!");
    TIMER_start(5); // buzzer handled on Control ECU
}


/************************* Helper: Check password with Control ECU *************************/
uint8_t CheckPasswordWithControl(uint8_t *pass)
{
    COMM_SendCommand(CMD_SEND_PASSWORD);
    COMM_SendMessage(pass);

    uint8_t response = COMM_ReceiveCommand();

    if (response == CMD_PASSWORD_CORRECT)
        return 1;
    else
        return 0;
}



/************************* Step 4: Change Password *************************/
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
            _delay_ms(800);
        }
    }

    /* Lockout */
    COMM_SendCommand(CMD_ALARM);
    LCD_clearScreen();
    LCD_displayString("LOCKED!");
    TIMER_start(5);
}



/************************* Step 5: Set Auto-Lock Timeout *************************/
void SetAutoLockTimeout(void)
{
    uint8_t pass[PASS_LENGTH + 1];

    uint8_t timeout = 5;

    while (1)
    {
        LCD_clearScreen();
        LCD_displayString("Timeout: ");
        timeout = POT_getValue(5, 30);  // Get value from potentiometer in range 5-30(not implemented yet)
        LCD_intgerToString(timeout);
        LCD_displayString(" sec");

        LCD_moveCursor(2,0);
        LCD_displayString("Press = to save");

        if (KEYPAD_getPressedKey() == '=')
            break;
    }

    /* Confirm by password */
    LCD_clearScreen();
    LCD_displayString("Enter Pass:");
    EnterPassword(pass);

    if (CheckPasswordWithControl(pass))
    {
        COMM_SendCommand(CMD_SEND_TIMEOUT);
        COMM_SendByte(timeout);

        if (COMM_ReceiveCommand() == CMD_ACK)
        {
            LCD_clearScreen();
            LCD_displayString("Saved!");
            _delay_ms(1000);
        }
    }
    else
    {
        LCD_clearScreen();
        LCD_displayString("Wrong!");
        _delay_ms(800);
    }
}

