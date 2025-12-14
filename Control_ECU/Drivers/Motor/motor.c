#include "motor.h"
#include "tm4c123gh6pm.h"


#define CLK_FREQUENCY 16000000

volatile uint8_t Motor_state = 0; // 0 = closed, 1 = opened

/* 
  -2 pins to output to the L298N (the H bridge) for logic handling 
  -gonna use motor A driver only thats why we are gonna use pins IN1 & IN2 only in the logic
*/

void init_LEDs(void) {
    SYSCTL_RCGCGPIO_R |= (1 << 5);        // Enable clock for Port F
    while(!(SYSCTL_PRGPIO_R & (1 << 5))); // Wait until Port F is ready

    GPIO_PORTF_DIR_R |= 0x0E;  // PF1, PF2, PF3 as output
    GPIO_PORTF_DEN_R |= 0x0E;  // Digital enable PF1, PF2, PF3
    GPIO_PORTF_DATA_R &= ~0x0E; // Turn all LEDs off initially
}

void toggle_LED(uint8_t led_pin) {
    GPIO_PORTF_DATA_R ^= led_pin;  // Toggle specific LED
}


void init_Motor(void){ 
  
  //***********************init port B pins 2 & 3 ****************************//
  SYSCTL_RCGCGPIO_R |= (1<<1);
  //waiting until the port is ready, SYSCTL_PRGPIO_R ->peripheral ready Gpio register
  while ((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R1) == 0) {}
  GPIO_PORTB_DEN_R |= ((1<<2) | (1<<3));
  GPIO_PORTB_DIR_R |= ((1<<2) | (1<<3));
  
  
  //***********************init timer 1*********************************//
  SYSCTL_RCGCTIMER_R |= (1<<1); //enable clk to timer 1
  while ((SYSCTL_PRTIMER_R & (1 << 1)) == 0); //wait till its set up
  
  TIMER1_CTL_R &= ~(1 << 0); //disable timer before configuration
  TIMER1_CFG_R = 0x0; //enable it as a 32-bit timer 
  TIMER1_TAMR_R = 0x1; //enable one shot mode!!
  
  TIMER1_ICR_R = 0x01; //disable interrupts
  TIMER1_IMR_R |= 0x01; //enable interrupts 
  NVIC_EN0_R |= (1 << 21); // enable TIMER1A's only interrupt in NVIC!!!
  init_LEDs();
}

//NOTE: IN1 ->PB2 & IN2 ->PB3 !!
void open_door(void){
  Motor_state = 1; //change mmotor state to opening 
  GPIO_PORTB_DATA_R |=  (1<<2); //IN1 = 1
  GPIO_PORTB_DATA_R &= ~(1<<3); //IN2 = 0
  TIMER1_CTL_R = 0x1; //start timer 
  for(int i = 0; i<5000000;i++){} //delay to keep the motor mocing for a while 
  GPIO_PORTB_DATA_R &= ~(1<<2);
}
void close_door(void){
  Motor_state = 0; //change state to closed
  
  GPIO_PORTB_DATA_R &= ~(1<<2); //IN1 = 0
  GPIO_PORTB_DATA_R |=  (1<<3); //IN2 = 1
  toggle_LED(1 << 3);
  for(int i = 0; i<5000000;i++){} //delay to keep the motor mocing for a while
  GPIO_PORTB_DATA_R &= ~(1<<3);
  
}

void Timer1A_Handler(void){
  TIMER1_ICR_R = 0x1; // clear interrupt flag
  close_door();
  
}



//Note: auto_lockoutSeconds is an integer number representing the number of seconds the door should be opened for !!
void start_Motor(int auto_lockoutSeconds){
  
  init_Motor();
  
  TIMER1_CTL_R &= ~(1 << 0); //disable timer before configuration
  TIMER1_ICR_R = 0x01; //clear interrupt flag
  
  TIMER1_TAILR_R = (auto_lockoutSeconds * CLK_FREQUENCY) -1;
  
  open_door();
  toggle_LED(1 << 2);
}
