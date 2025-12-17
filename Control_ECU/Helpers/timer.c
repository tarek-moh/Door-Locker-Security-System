#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "timer.h"

void SysTick_Init(uint32_t reload, uint8_t mode)
{
    interruptMode = mode;

    NVIC_ST_CTRL_R = 0;               // Disable SysTick
    NVIC_ST_RELOAD_R = reload - 1;    // Set reload value
    NVIC_ST_CURRENT_R = 0;            // Clear current

    if (mode == SYSTICK_INT)
    {
        NVIC_ST_CTRL_R = 0x07;        // ENABLE | TICKINT | CLK_SRC
    }
    else
    {
        NVIC_ST_CTRL_R = 0x05;        // ENABLE | CLK_SRC (no interrupt)
    }
}

void DelayMs(uint32_t ms)
{
    if (interruptMode == SYSTICK_NOINT)
    {
        // POLLING MODE - actively check COUNT flag
        for (uint32_t i = 0; i < ms; i++)
        {
            while ((NVIC_ST_CTRL_R  & (1 << 16)) == 0);
        }
    }
}

/*
    SysTick Interrupt Handler
    Overflow is automatically handled by C
*/
void SystickHandler(void)
{
    msTicks++;
}