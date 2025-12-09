#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "systick.h"

volatile uint32_t msTicks = 0;
static uint8_t interruptMode = 0;

void SysTick_Init(uint32_t reload, uint8_t mode)
{
    interruptMode = mode;

    NVIC_ST_CTRL_R = 0;               // disable systick
    NVIC_ST_RELOAD_R = reload - 1;    // set reload
    NVIC_ST_CURRENT_R = 0;            // clear current

    if (mode == SYSTICK_INT)
    {
        NVIC_ST_CTRL_R = 0x07;        // enable | interrupt | system clock
    }
    else
    {
        NVIC_ST_CTRL_R = 0x05;        // enable | system clock
    }
}

void DelayMs(uint32_t ms)
{
    if (interruptMode == SYSTICK_NOINT)
    {
        for (uint32_t i = 0; i < ms; i++)
        {
            // wait for CNTFLAG
            while ((NVIC_ST_CTRL_R & (1 << 16)) == 0);
        }
    }
    else
    {
        uint32_t start = msTicks;
        while ((msTicks - start) < ms);
    }
}

uint32_t SysTick_GetMs(void)
{
    return msTicks;
}

// Correct ISR Name
void SysTick_Handler(void)
{
    msTicks++;        // increment 1ms counter
}
