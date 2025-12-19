/******************************************************************************
 * File: hmi.h
 * Module: HMI (Human-Machine Interface) Application Layer
 * Description: Door Locker Security System - HMI ECU Logic
 * Date: December 18, 2025
 ******************************************************************************/

#ifndef HMI_H_
#define HMI_H_

#include <stdint.h>

/******************************************************************************
 *                              Definitions                                    *
 ******************************************************************************/

/* Password Configuration */
#define PASSWORD_LENGTH         4
#define MAX_PASSWORD_ATTEMPTS   3

/* Timeout Configuration (in seconds) */
#define MIN_TIMEOUT_SEC         5
#define MAX_TIMEOUT_SEC         30
#define DEFAULT_TIMEOUT_SEC     10

/* Door Operation Timings (in seconds) */
#define DOOR_UNLOCK_TIME        15    /* Motor unlocking time */
#define DOOR_LOCK_TIME          15    /* Motor locking time */

/* Lockout/Alarm Duration */
#define LOCKOUT_DURATION_SEC    60    /* 1 minute lockout */

/* Menu Key Definitions */
#define KEY_OPEN_DOOR           'A'    // Changed from '+'
#define KEY_CHANGE_PASSWORD     'B'    // Changed from '-'
#define KEY_SET_TIMEOUT         'C'    // Changed from '*'
#define KEY_CANCEL              'D'    // Changed from 'C'
#define KEY_CONFIRM             '#'    // Keep as is

/******************************************************************************
 *                         Function Prototypes                                 *
 ******************************************************************************/

/*
 * Description: Initialize HMI system
 * - Initializes all peripherals (LCD, Keypad, LED, Potentiometer, UART)
 * - Performs initial handshake with Control ECU
 * - Displays welcome message
 * Parameters: None
 * Returns: None
 */
void HMI_Init(void);

/*
 * Description: Main HMI task - implements state machine
 * - Must be called continuously in main loop
 * - Handles all user interactions and state transitions
 * Parameters: None
 * Returns: None
 */
void HMI_Task(void);

/*
 * Description: Get password input from user via keypad
 * - Accepts only numeric digits (0-9)
 * - Displays '*' for each entered digit
 * - Exactly PASSWORD_LENGTH digits required
 * Parameters:
 *   - buffer: Array to store password (must be PASSWORD_LENGTH + 1 for null terminator)
 * Returns: None
 */
void HMI_GetPasswordInput(char* buffer);

/*
 * Description: Send password to Control ECU and verify
 * Parameters:
 *   - password: Password string to verify
 * Returns: 1 if password correct, 0 if incorrect
 */
uint8_t HMI_VerifyPassword(const char* password);

/*
 * Description: Send password to Control ECU for storage
 * Parameters:
 *   - password: Password string to save
 * Returns: 1 if successful, 0 if failed
 */
void HMI_SavePassword(const char* password);

/*
 * Description: Handle first-time password setup
 * - Prompts user to enter password twice
 * - Verifies both entries match
 * - Saves password to Control ECU
 * Parameters: None
 * Returns: 1 if successful, 0 if failed
 */
uint8_t HMI_SetupPassword(void);

/*
 * Description: Display main menu on LCD
 * Parameters: None
 * Returns: None
 */
void HMI_ShowMainMenu(void);

/*
 * Description: Handle door opening operation
 * - Request password from user
 * - Verify with Control ECU
 * - Display door operation status
 * - Handle failed attempts and lockout
 * Parameters: None
 * Returns: 1 if successful, 0 if failed/locked out
 */
uint8_t HMI_HandleOpenDoor(void);

/*
 * Description: Handle password change operation
 * - Request old password
 * - Verify old password
 * - Request new password twice
 * - Save to Control ECU
 * Parameters: None
 * Returns: 1 if successful, 0 if failed
 */
uint8_t HMI_HandleChangePassword(void);

/*
 * Description: Handle timeout setting via potentiometer
 * - Display live potentiometer reading (5-30 seconds)
 * - Wait for user confirmation (#) or cancel (C)
 * - Request password before saving
 * Parameters: None
 * Returns: 1 if successful, 0 if cancelled/failed
 */
uint8_t HMI_HandleSetTimeout(void);

/*
 * Description: Display door operation progress
 * - Shows "Unlocking...", "Door Open", "Locking..." messages
 * - Displays countdown timer
 * Parameters:
 *   - status: Status message to display
 *   - seconds: Countdown value (0 if no countdown)
 * Returns: None
 */
void HMI_DisplayDoorStatus(const char* status, uint16_t seconds);

/*
 * Description: Handle lockout state
 * - Display lockout message with countdown
 * - Trigger alarm on Control ECU
 * - Wait for lockout period to expire
 * Parameters: None
 * Returns: None
 */
void HMI_HandleLockout(void);

/*
 * Description: Display message on LCD (1 or 2 lines)
 * Parameters:
 *   - line1: First line message (max 16 chars)
 *   - line2: Second line message (can be NULL for single line)
 * Returns: None
 */
void HMI_DisplayMessage(const char* line1, const char* line2);

/*
 * Description: Clear any pending keypad inputs
 * Parameters: None
 * Returns: None
 */
void HMI_ClearKeypadBuffer(void);

/*
 * Description: Displays connected when the handshake is performed
 * Parameters: None
 * Returns: None
 */
void DisplayConnection();

#endif /* HMI_H_ */