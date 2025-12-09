 /******************************************************************************
 * File: gpio.h
 * Module: GPIO (General Purpose Input/Output)
 * Description: Header file for TM4C123GH6PM GPIO Driver
 * Author: Antigravity
 * Date: December 8, 2025
 ******************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

/******************************************************************************
 *                              Definitions                                    *
 ******************************************************************************/

/* Logic Levels */
#define LOW         0
#define HIGH        1

/* Directions */
#define INPUT       0
#define OUTPUT      1

/* Ports */
#define PORTA_ID    0
#define PORTB_ID    1
#define PORTC_ID    2
#define PORTD_ID    3
#define PORTE_ID    4
#define PORTF_ID    5

/* Pins */
#define PIN0_ID     0
#define PIN1_ID     1
#define PIN2_ID     2
#define PIN3_ID     3
#define PIN4_ID     4
#define PIN5_ID     5
#define PIN6_ID     6
#define PIN7_ID     7

/******************************************************************************
 *                         Function Prototypes                                 *
 ******************************************************************************/

/*
 * Description: Initializes a specific GPIO pin with the specified direction.
 *              - Enables the clock for the port.
 *              - Unlocks the pin if it is locked (e.g., PF0, PD7).
 *              - Configures as Digital I/O.
 *              - Sets direction (Input/Output).
 *              - For Inputs: Enables Pull-Up resistor by default (can be modified).
 * Parameters:
 *   - port_num: Port ID (0-5)
 *   - pin_num: Pin ID (0-7)
 *   - direction: INPUT or OUTPUT
 */
void GPIO_Init(uint8_t port_num, uint8_t pin_num, uint8_t direction);

/*
 * Description: Write a value (HIGH/LOW) to a specific pin.
 * Parameters:
 *   - port_num: Port ID
 *   - pin_num: Pin ID
 *   - value: HIGH or LOW
 */
void GPIO_WritePin(uint8_t port_num, uint8_t pin_num, uint8_t value);

/*
 * Description: Read the value of a specific pin.
 * Parameters:
 *   - port_num: Port ID
 *   - pin_num: Pin ID
 * Returns: HIGH or LOW
 */
uint8_t GPIO_ReadPin(uint8_t port_num, uint8_t pin_num);

/*
 * Description: Toggle the value of a specific pin.
 * Parameters:
 *   - port_num: Port ID
 *   - pin_num: Pin ID
 */
void GPIO_TogglePin(uint8_t port_num, uint8_t pin_num);

/*
 * Description: Enable or Disable internal Pull-Up Resistor.
 */
void GPIO_SetPullUp(uint8_t port_num, uint8_t pin_num, uint8_t enable);

#endif /* GPIO_H_ */
