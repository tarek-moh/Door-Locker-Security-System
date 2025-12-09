 /******************************************************************************
 * File: gpio.c
 * Module: GPIO (General Purpose Input/Output)
 * Description: Source file for TM4C123GH6PM GPIO Driver
 * Author: Antigravity
 * Date: December 8, 2025
 ******************************************************************************/

#include "gpio.h"
#include "../../tm4c123gh6pm.h"

/******************************************************************************
 *                              Macros & Helpers                               *
 ******************************************************************************/

#define GPIO_LOCK_KEY           0x4C4F434B

/* Helper: Get Base Address of Port */
static volatile uint32_t* GetPortBase(uint8_t port_num)
{
    switch(port_num)
    {
        case PORTA_ID: return (volatile uint32_t*)0x40004000;
        case PORTB_ID: return (volatile uint32_t*)0x40005000;
        case PORTC_ID: return (volatile uint32_t*)0x40006000;
        case PORTD_ID: return (volatile uint32_t*)0x40007000;
        case PORTE_ID: return (volatile uint32_t*)0x40024000;
        case PORTF_ID: return (volatile uint32_t*)0x40025000;
        default:       return 0;
    }
}

/******************************************************************************
 *                         Function Definitions                                *
 ******************************************************************************/

void GPIO_Init(uint8_t port_num, uint8_t pin_num, uint8_t direction)
{
    volatile uint32_t delay;
    
    /* 1. Enable Clock for the Port */
    SYSCTL_RCGCGPIO_R |= (1 << port_num);
    delay = SYSCTL_RCGCGPIO_R; /* Dummy read */
    
    /* Base addresses offsets (TM4C123GH6PM) */
    /* We use offsets from the base pointer calculated manually or use the header definitions if consistent */
    /* Using the standard header definitions with offset calculation is tricky without a mapping array. */
    /* Let's use the explicit register definitions from tm4c header with a switch or mapping. */
    /* BUT, since we have the header, we can map ports to their registers logic. */
    
    /* To keep it clean and fast, we'll use a switch for the critical configuration steps that need base addresses 
       OR use the bit-banding or offset method.
       Given the provided tm4c header usually has definitions like GPIO_PORTA_DATA_R.
    */
    
    /* Simple mapping approach for registers */
    volatile uint32_t *CR_Ptr;
    volatile uint32_t *AMSEL_Ptr;
    volatile uint32_t *PCTL_Ptr;
    volatile uint32_t *DIR_Ptr;
    volatile uint32_t *AFSEL_Ptr;
    volatile uint32_t *DEN_Ptr;
    volatile uint32_t *LOCK_Ptr;
    volatile uint32_t *PUR_Ptr;

    switch(port_num)
    {
        case PORTA_ID:
            LOCK_Ptr = &GPIO_PORTA_LOCK_R; CR_Ptr = &GPIO_PORTA_CR_R;
            AMSEL_Ptr = &GPIO_PORTA_AMSEL_R; PCTL_Ptr = &GPIO_PORTA_PCTL_R;
            DIR_Ptr = &GPIO_PORTA_DIR_R; AFSEL_Ptr = &GPIO_PORTA_AFSEL_R;
            DEN_Ptr = &GPIO_PORTA_DEN_R; PUR_Ptr = &GPIO_PORTA_PUR_R;
            break;
        case PORTB_ID:
            LOCK_Ptr = &GPIO_PORTB_LOCK_R; CR_Ptr = &GPIO_PORTB_CR_R;
            AMSEL_Ptr = &GPIO_PORTB_AMSEL_R; PCTL_Ptr = &GPIO_PORTB_PCTL_R;
            DIR_Ptr = &GPIO_PORTB_DIR_R; AFSEL_Ptr = &GPIO_PORTB_AFSEL_R;
            DEN_Ptr = &GPIO_PORTB_DEN_R; PUR_Ptr = &GPIO_PORTB_PUR_R;
            break;
        case PORTC_ID:
            LOCK_Ptr = &GPIO_PORTC_LOCK_R; CR_Ptr = &GPIO_PORTC_CR_R;
            AMSEL_Ptr = &GPIO_PORTC_AMSEL_R; PCTL_Ptr = &GPIO_PORTC_PCTL_R;
            DIR_Ptr = &GPIO_PORTC_DIR_R; AFSEL_Ptr = &GPIO_PORTC_AFSEL_R;
            DEN_Ptr = &GPIO_PORTC_DEN_R; PUR_Ptr = &GPIO_PORTC_PUR_R;
            break;
        case PORTD_ID:
            LOCK_Ptr = &GPIO_PORTD_LOCK_R; CR_Ptr = &GPIO_PORTD_CR_R;
            AMSEL_Ptr = &GPIO_PORTD_AMSEL_R; PCTL_Ptr = &GPIO_PORTD_PCTL_R;
            DIR_Ptr = &GPIO_PORTD_DIR_R; AFSEL_Ptr = &GPIO_PORTD_AFSEL_R;
            DEN_Ptr = &GPIO_PORTD_DEN_R; PUR_Ptr = &GPIO_PORTD_PUR_R;
            break;
        case PORTE_ID:
            LOCK_Ptr = &GPIO_PORTE_LOCK_R; CR_Ptr = &GPIO_PORTE_CR_R;
            AMSEL_Ptr = &GPIO_PORTE_AMSEL_R; PCTL_Ptr = &GPIO_PORTE_PCTL_R;
            DIR_Ptr = &GPIO_PORTE_DIR_R; AFSEL_Ptr = &GPIO_PORTE_AFSEL_R;
            DEN_Ptr = &GPIO_PORTE_DEN_R; PUR_Ptr = &GPIO_PORTE_PUR_R;
            break;
        case PORTF_ID:
            LOCK_Ptr = &GPIO_PORTF_LOCK_R; CR_Ptr = &GPIO_PORTF_CR_R;
            AMSEL_Ptr = &GPIO_PORTF_AMSEL_R; PCTL_Ptr = &GPIO_PORTF_PCTL_R;
            DIR_Ptr = &GPIO_PORTF_DIR_R; AFSEL_Ptr = &GPIO_PORTF_AFSEL_R;
            DEN_Ptr = &GPIO_PORTF_DEN_R; PUR_Ptr = &GPIO_PORTF_PUR_R;
            break;
        default: return;
    }

    /* 2. Unlock Pin if necessary (PF0, PD7) */
    if( (port_num == PORTF_ID && pin_num == PIN0_ID) || 
        (port_num == PORTD_ID && pin_num == PIN7_ID) )
    {
        *LOCK_Ptr = GPIO_LOCK_KEY;
        *CR_Ptr |= (1 << pin_num);
    }
    /* JTAG pins are on PC0-PC3, normally we shouldn't unlock/touch them unless needed as GPIO.
       But if requested, we would unlock. For now, only PF0/PD7 are critical standard locked pins. */

    /* 3. Disable Analog Function */
    *AMSEL_Ptr &= ~(1 << pin_num);

    /* 4. Clear PCTL (GPIO Function) */
    *PCTL_Ptr &= ~(0xF << (pin_num * 4));

    /* 5. Set Direction */
    if(direction == OUTPUT)
    {
        *DIR_Ptr |= (1 << pin_num);
    }
    else
    {
        *DIR_Ptr &= ~(1 << pin_num);
    }

    /* 6. Disable Alternate Function */
    *AFSEL_Ptr &= ~(1 << pin_num);

    /* 7. Enable Digital I/O */
    *DEN_Ptr |= (1 << pin_num);
}

void GPIO_WritePin(uint8_t port_num, uint8_t pin_num, uint8_t value)
{
    volatile uint32_t *Data_Ptr;
    
    /* Calculate Data Register Address with Masking for atomic access */
    /* Address = Base + 0x3FC (DATA) if we don't use masking */
    /* But standard TM4C allows reading/writing bits by address offset. */
    /* To keep it simple and generic with standard header macros, we'll read/modify/write the full data register 
       or use the bit-specific mask address which is (Base + (mask << 2)).
    */
    
    uint32_t PortBase = 0;
    switch(port_num)
    {
        case PORTA_ID: PortBase = 0x40004000; break;
        case PORTB_ID: PortBase = 0x40005000; break;
        case PORTC_ID: PortBase = 0x40006000; break;
        case PORTD_ID: PortBase = 0x40007000; break;
        case PORTE_ID: PortBase = 0x40024000; break;
        case PORTF_ID: PortBase = 0x40025000; break;
        default: return;
    }
    
    /* Using masking feature: Bit-specific address */
    /* Address = Base + (BitMask << 2) */
    Data_Ptr = (volatile uint32_t *)(PortBase + ((1 << pin_num) << 2));
    
    /* If we write to this specific address, we can write 0xFF or 0x00, 
       any bit set in the mask (pin_num) will be updated. 
       Since we address ONLY this pin, we don't affect others. */
    
    if(value == HIGH)
    {
        *Data_Ptr = (1 << pin_num); /* OR simply 0xFF because of the mask address */
    }
    else
    {
        *Data_Ptr = 0;
    }
}

uint8_t GPIO_ReadPin(uint8_t port_num, uint8_t pin_num)
{
    uint32_t PortBase = 0;
     switch(port_num)
    {
        case PORTA_ID: PortBase = 0x40004000; break;
        case PORTB_ID: PortBase = 0x40005000; break;
        case PORTC_ID: PortBase = 0x40006000; break;
        case PORTD_ID: PortBase = 0x40007000; break;
        case PORTE_ID: PortBase = 0x40024000; break;
        case PORTF_ID: PortBase = 0x40025000; break;
        default: return LOW;
    }
    
    volatile uint32_t *Data_Ptr = (volatile uint32_t *)(PortBase + ((1 << pin_num) << 2));
    
    if(*Data_Ptr != 0)
    {
        return HIGH;
    }
    return LOW;
}

void GPIO_TogglePin(uint8_t port_num, uint8_t pin_num)
{
    volatile uint32_t *Data_Ptr_Full;
    switch(port_num)
    {
        case PORTA_ID: Data_Ptr_Full = &GPIO_PORTA_DATA_R; break;
        case PORTB_ID: Data_Ptr_Full = &GPIO_PORTB_DATA_R; break;
        case PORTC_ID: Data_Ptr_Full = &GPIO_PORTC_DATA_R; break;
        case PORTD_ID: Data_Ptr_Full = &GPIO_PORTD_DATA_R; break;
        case PORTE_ID: Data_Ptr_Full = &GPIO_PORTE_DATA_R; break;
        case PORTF_ID: Data_Ptr_Full = &GPIO_PORTF_DATA_R; break;
        default: return;
    }
    
    *Data_Ptr_Full ^= (1 << pin_num);
}

void GPIO_SetPullUp(uint8_t port_num, uint8_t pin_num, uint8_t enable)
{
    volatile uint32_t *PUR_Ptr;
     switch(port_num)
    {
        case PORTA_ID: PUR_Ptr = &GPIO_PORTA_PUR_R; break;
        case PORTB_ID: PUR_Ptr = &GPIO_PORTB_PUR_R; break;
        case PORTC_ID: PUR_Ptr = &GPIO_PORTC_PUR_R; break;
        case PORTD_ID: PUR_Ptr = &GPIO_PORTD_PUR_R; break;
        case PORTE_ID: PUR_Ptr = &GPIO_PORTE_PUR_R; break;
        case PORTF_ID: PUR_Ptr = &GPIO_PORTF_PUR_R; break;
        default: return;
    }
    
    if(enable)
        *PUR_Ptr |= (1 << pin_num);
    else
        *PUR_Ptr &= ~(1 << pin_num);
}
