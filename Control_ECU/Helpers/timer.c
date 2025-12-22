#include <stdint.h>
#include "../../Common/MCAL/tm4c123gh6pm.h"
#include "timer.h"

volatile uint32_t msTicks = 0;

void SysTick_Init(uint32_t reload, uint8_t mode)
{

}

void DelayMs(uint32_t ms)
{

}

/*
    SysTick Interrupt Handler
    Overflow is automatically handled by C
*/
void SystickHandler(void)
{
   
}

uint32_t GetTicks() {
  return msTicks;
}