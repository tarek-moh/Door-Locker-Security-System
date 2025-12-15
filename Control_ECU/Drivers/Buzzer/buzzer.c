#include "buzzer.h"
#include "../../../Common/MCAL/tm4c123gh6pm.h"


#define TOTAL_BEEPS 3
#define CYCLES_PER_MS 16000  // 16 MHz / 1000 = 16000 cycles per ms

// Duration arrays (milliseconds)
uint32_t on_durations[TOTAL_BEEPS]  = {600, 700, 850};  // buzzer ON
uint32_t off_durations[TOTAL_BEEPS] = {300, 300, 300};  // buzzer OFF

// State variables
volatile uint8_t buzzer_state = 0; // 0 = off, 1 = on
volatile uint8_t beep_index = 0;

//NOTE : TIMER by default counts down !!
void init_Buzzer(void){
  //**********************************************PORT B init*****************************************************//
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
  
  //**********************************************TIMER0 init****************************************************//
  SYSCTL_RCGCTIMER_R |=(1<<0); //enable clock to timer 0
   while ((SYSCTL_PRTIMER_R & (1 << 0)) == 0); //wait till its set up
   TIMER0_CTL_R &= ~(1 << 0);//disable timer before configuration
   
   TIMER0_CFG_R = 0x00000000; //configure it to be 32-bit timer
   TIMER0_TAMR_R = 0x1; // 0x1 = One-Shot Timer modee counts once only
   //interupt settings
   TIMER0_IMR_R |= 0x01; // enable timeout interrupt
   NVIC_EN0_R |= (1 << 19); // enable TIMER0A's only interrupt in NVIC
}




void toggle_Buzzer(void) {
    if (buzzer_state == 1) {
        // --- BUZZER WAS ON, TURN it OFF ---
        GPIO_PORTB_DATA_R &= ~(1 << 0); 
        buzzer_state = 0;

        // Load OFF duration
        TIMER0_TAILR_R = (off_durations[beep_index] * CYCLES_PER_MS) - 1;
        TIMER0_CTL_R |= 0x01; // Restart Timer

    } else {
        // --- BUZZER WAS OFF, CHECK NEXT ---
        beep_index++; 

        if (beep_index < TOTAL_BEEPS) {
            // Start next beep
            GPIO_PORTB_DATA_R |= (1 << 0); 
            buzzer_state = 1;

            // Load ON duration
            TIMER0_TAILR_R = (on_durations[beep_index] * CYCLES_PER_MS) - 1;
            TIMER0_CTL_R |= 0x01; // Restart Timer
        } else {
            // --- DONE ---
            TIMER0_CTL_R &= ~(1 << 0);      // Stop Timer
            GPIO_PORTB_DATA_R &= ~(1 << 0); // Ensure Pin LOW
        }
    }
}


void Timer0A_Handler(void) {
    TIMER0_ICR_R = 0x01; // Clear interrupt flag
    toggle_Buzzer();     //handle interrupt in a separated func!!
}

void Buzzer_Start(void) {
    init_Buzzer();
    TIMER0_CTL_R &= ~(1 << 0); //Stop if running for safety
    TIMER0_ICR_R = 0x01;       //Clear pending interrupts for safety
    
    beep_index = 0;
    buzzer_state = 1; 

    // Load first ON duration
    TIMER0_TAILR_R = (on_durations[beep_index] * CYCLES_PER_MS) - 1;
    
    // Turn ON
    GPIO_PORTB_DATA_R |= (1 << 0);
    
    // GO
    TIMER0_CTL_R |= 0x01; //the timers interrupt handler will get invoked when the first time from the array period have elapsed
}

