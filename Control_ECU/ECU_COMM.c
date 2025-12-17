#include "comm_interface.h"
#include "Drivers/Eeprom/eeprom.h"
#include "Drivers/Motor/motor.h"
#include "Drivers/Buzzer/buzzer.h"
#include "Helpers/timer.h"
#include "Helpers/software_reset.h"

#define MAX_ATTEMPTS 3
#define TIMEOUT_MS 100

void static inline WaitForAck(void);
void static inline IncrementAttempts(uint8_t *attempts);
void static inline ResetAttempts(uint8_t *attempts);

int main(void) {
    // Initialize the communication path
    COMM_Init();

    // Initialize the timer
    SysTick_Init(16000, SYSTICK_INT);

    // Send a command to the Control_ECU that the HMI_ECU is reading for communication
    COMM_SendCommand(CMD_READY);
    while (COMM_ReceiveCommand() != CMD_READY) { }

    uint8_t incorrectAttempts = 0;
    uint8_t input[10];

    for (;;) {
        const uint8_t command = COMM_ReceiveCommand();

        switch (command) {
            case CMD_SEND_PASSWORD:
                COMM_ReceiveMessage(input);
                bool isCorrect = compare_Passwords(input);

                if (isCorrect == true) {
                    ResetAttempts(&incorrectAttempts);
                    COMM_SendCommand(CMD_PASSWORD_CORRECT);
                } else {
                    IncrementAttempts(&incorrectAttempts);
                    COMM_SendCommand(CMD_PASSWORD_WRONG);
                }
                WaitForAck();
                break;
            case CMD_DOOR_UNLOCK:
                start_Motor(get_AutoLockTimeout());
                COMM_SendCommand(CMD_ACK);
                break;
            case CMD_CHANGE_PASSWORD:
                COMM_ReceiveMessage(input);
                change_Password(input);
                COMM_SendCommand(CMD_ACK);
                break;
            case CMD_SET_TIMEOUT:
                COMM_ReceiveMessage(input);
                if (set_AutoLockTimeout(input[0])) {
                    COMM_SendCommand(CMD_SUCCESS);
                } else {
                    // Must be >= 5 && <= 30
                    COMM_SendCommand(CMD_FAIL);
                }
                break;
            default:
                COMM_SendCommand(CMD_UNKNOWN);
                break;
        }
    }
    return 0;
}

// Loop indefinitely until a response is made
void inline WaitForAck(void) {
    // record start time
    uint32_t start = msTicks;

    while (COMM_ReceiveCommand() != CMD_ACK) {
        if ((msTicks - start) >= TIMEOUT_MS)
            // timeout
            ResetTiva();
    }
}

void inline IncrementAttempts(uint8_t *attempts) {
    if (*attempts < MAX_ATTEMPTS) {
        ++(*attempts);
    } else {
        Buzzer_Start();
    }
}

void inline ResetAttempts(uint8_t *attempts) {
    *attempts = 0;
    // todo: Turn off buzzer
}