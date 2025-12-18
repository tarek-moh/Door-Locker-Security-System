/******************************************************************************
 * File: led.c
 * Module: LED Driver for TM4C123GH6PM
 * Description: Controls the onboard RGB LED using DIO driver
 * Author: Antigravity
 * Date: December 8, 2025
 ******************************************************************************/

#include "led.h"
#include "../../MCAL/gpio/gpio.h"

/*
 * RGB LED pins on TM4C LaunchPad:
 *   RED   -> PF1
 *   BLUE  -> PF2
 *   GREEN -> PF3
 */

#define LED_PORT   PORTF_ID
#define LED_RED_PIN    PIN1_ID
#define LED_BLUE_PIN   PIN2_ID
#define LED_GREEN_PIN  PIN3_ID

void LED_init(void)
{
    /* Initialize RGB LED pins as OUTPUT */
    GPIO_Init(LED_PORT, LED_RED_PIN, OUTPUT);
    GPIO_Init(LED_PORT, LED_GREEN_PIN, OUTPUT);
    GPIO_Init(LED_PORT, LED_BLUE_PIN, OUTPUT);

    /* Turn LED OFF initially */
    GPIO_WritePin(LED_PORT, LED_RED_PIN, LOW);
    GPIO_WritePin(LED_PORT, LED_GREEN_PIN, LOW);
    GPIO_WritePin(LED_PORT, LED_BLUE_PIN, LOW);
}

void LED_setOn(uint8_t color)
{
    /* Turn all LEDs OFF first */
    GPIO_WritePin(LED_PORT, LED_RED_PIN, LOW);
    GPIO_WritePin(LED_PORT, LED_GREEN_PIN, LOW);
    GPIO_WritePin(LED_PORT, LED_BLUE_PIN, LOW);

    /* Set selected color */
    switch(color)
    {
        case LED_RED:
            GPIO_WritePin(LED_PORT, LED_RED_PIN, HIGH);
            break;

        case LED_GREEN:
            GPIO_WritePin(LED_PORT, LED_GREEN_PIN, HIGH);
            break;

        case LED_BLUE:
            GPIO_WritePin(LED_PORT, LED_BLUE_PIN, HIGH);
            break;

        case LED_OFF:
        default:
            /* Already turned OFF above */
            break;
    }
}
