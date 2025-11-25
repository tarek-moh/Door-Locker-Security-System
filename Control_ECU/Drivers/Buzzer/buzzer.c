#include "buzzer.h"
//#include "../../Common/MCAL/tm4c123gh6pm.h"
#include "tm4c123gh6pm.h"


#define TOTAL_BEEPS 3
#define TIMER_FREQ 16000000    // System clock = 16 MHz

// Duration arrays (milliseconds)
uint32_t on_durations[TOTAL_BEEPS]  = {500, 500, 500};  // buzzer ON
uint32_t off_durations[TOTAL_BEEPS] = {200, 300, 400};  // buzzer OFF

// State variables
volatile uint8_t buzzer_state = 0; // 0 = off, 1 = on
volatile uint8_t beep_index = 0;


void init_Buzzer(void){
  //********************PORT B init****************************************//
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // enable Port B 
  //waiting until the port is ready
  while ((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R1) == 0) {}
  GPIO_PORTB_DIR_R |= (1 << 0); // set PB0 as output pin
  GPIO_PORTB_DEN_R |= (1<<0); //enable digital signals for PB0
  
  /*This tells the microcontroller whether the pin should act as normal GPIO 
  or some alternate function (like UART, PWM, I2C, etc.).*/
  GPIO_PORTB_AFSEL_R &= ~(1 << 0); // disable alternate functions..to make sure its working as GPIO not other function
  /*
When a pin is in alternate function mode, PCTL selects which alternate function it uses.
  Bits 3:0    -> PB0
  Bits 7:4    -> PB1
  Bits 11:8   -> PB2
  .... so on !!
*/
  GPIO_PORTB_PCTL_R &= ~0xF;       // clear PCTL(Port Control Register) for PB0
  
  //******************************TIMER0 init********************************//
  SYSCTL_RCGCTIMER_R |=(1<<0); //enable clock to timer 0
   while ((SYSCTL_PRTIMER_R & (1 << 0)) == 0); //wait till its set up
   TIMER0_CTL_R &= ~(1 << 0);//disable timer before configuration
   
   TIMER0_CFG_R = 0x00000000; //configure it to be 32-bit timer
   TIMER0_TAMR_R = 0x1; // 0x1 = One-Shot Timer modee 
   //interupt settings
   TIMER0_IMR_R |= 0x01; // enable timeout interrupt
   NVIC_EN0_R |= (1 << 19); // enable TIMER0A only interrupt in NVIC
}


void Timer0A_Handler(void) {
    TIMER0_ICR_R = 0x01; // Clear interrupt flag

    if (buzzer_state) {
        // State 1: Buzzer was ON, now turn OFF
        GPIO_PORTB_DATA_R &= ~(1 << 0); // Turn OFF
        buzzer_state = 0;

        // Load OFF duration for the CURRENT beep_index
        TIMER0_TAILR_R = (off_durations[beep_index] * TIMER_FREQ) / 1000 - 1;
        
        // *** FIX 1: Restart the timer for the OFF duration ***
        TIMER0_CTL_R |= 0x01; 

    } else {
        // State 0: Buzzer was OFF, check if more beeps are needed
        
        // This is the end of the ON/OFF cycle for the CURRENT beep_index.
        // Move to the next beep index.
        beep_index++; 

        if (beep_index < TOTAL_BEEPS) {
            // New beep cycle starts 
            
            // Turn buzzer ON
            GPIO_PORTB_DATA_R |= (1 << 0); // Turn ON
            buzzer_state = 1;

            // Load ON duration for the NEW beep_index
            TIMER0_TAILR_R = (on_durations[beep_index] * TIMER_FREQ) / 1000 - 1;
            
            // *** FIX 2: Restart the timer for the ON duration ***
            TIMER0_CTL_R |= 0x01; 
        } else {
            // All beeps done 
            
            // Stop timer (this is correct for the final stop)
            TIMER0_CTL_R &= ~(1 << 0); 
            GPIO_PORTB_DATA_R &= ~(1 << 0); // Ensure buzzer is off
        }
    }
}

void toggle_Buzzer(void){
    beep_index = 0;
    buzzer_state = 1; // start with ON
    // Load first ON duration
    TIMER0_TAILR_R = (on_durations[beep_index] * TIMER_FREQ) / 1000 - 1;
    // Turn buzzer ON
    GPIO_PORTB_DATA_R |= (1 << 0);
    // Start timer
    TIMER0_CTL_R |= 0x01;
}

