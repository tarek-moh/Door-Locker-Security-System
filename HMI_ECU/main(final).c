/******************************************************************************
 * File: main.c
 * Module: Main Application
 * Description: Door Locker Security System - HMI ECU Main Entry Point
 * Author: Improved Implementation
 * Date: December 18, 2025
 ******************************************************************************/

#include "tm4c123gh6pm.h"
#include "APP/hmi.h"

/******************************************************************************
 *                              Main Function                                  *
 ******************************************************************************/

int main(void)
{
    /* Initialize HMI system - this handles all hardware initialization */
    HMI_Init();
    
    /* Main application loop - run HMI state machine continuously */
    while(1)
    {
        HMI_Task();
    }
    
    /* Should never reach here */
    return 0;
}