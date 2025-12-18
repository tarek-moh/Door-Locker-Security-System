#ifndef LED_H_
#define LED_H_

#include <stdint.h>

#define LED_OFF   0
#define LED_RED   1
#define LED_GREEN 2
#define LED_BLUE  3

/*
 * Initialize RGB LED pins.
 */
void LED_init(void);

/*
 * Set LED color.
 * color: LED_OFF, LED_RED, LED_GREEN, LED_BLUE
 */
void LED_setOn(uint8_t color);

#endif /* LED_H_ */


