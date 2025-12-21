 #include "../Common/HAL/comm_interface.h"
 #include "Drivers/Eeprom/eeprom.h"
 #include "Drivers/Motor/motor.h"
 #include "Drivers/Buzzer/buzzer.h"
 #include "Helpers/timer.h"
//#include "Helpers/software_reset.h"
 #include "../Common/MCAL/tm4c123gh6pm.h"

#define MAX_ATTEMPTS 3
#define TIMEOUT_MS 100

void static inline WaitForAck(void);
void static inline IncrementAttempts(uint8_t *attempts);
void static inline ResetAttempts(uint8_t *attempts);

//void init_LEDs(void) {
  //  SYSCTL_RCGCGPIO_R |= (1 << 5);        //enable clock for Port F
  //  while(!(SYSCTL_PRGPIO_R & (1 << 5))); //wait until Port F is ready

    //GPIO_PORTF_DIR_R |= 0x0E;  // PF1, PF2, PF3 as output
    //GPIO_PORTF_DEN_R |= 0x0E;  
    //GPIO_PORTF_DATA_R &= ~0x0E; // turn all LEDs off initially
//}

//void toggle_LED(uint8_t led_pin) {
  
  //  GPIO_PORTF_DATA_R &= ~0x0E; // turn all LEDs off initially
  //  GPIO_PORTF_DATA_R ^= led_pin;  //toggle specific led
//}

int main(void) {

     __asm("CPSIE I");  // set the I-bit in PRIMASK ? enables global interrupts
    // Initialize the communication path
    COMM_Init();
    init_LEDs();  //init leds debugging purposes
    // Initialize the timer
    SysTick_Init(16000, SYSTICK_INT);

    // Send a command to the Control_ECU that the HMI_ECU is reading for communication
    COMM_SendCommand(CMD_READY);
    while (COMM_ReceiveCommand() != CMD_READY) { }

    // Check if password is already initialized
    if (!is_password_init()) {
        COMM_SendCommand(CMD_INIT);
    }
    else
    {
        COMM_SendCommand(CMD_INIT);
    }

    // UARTprintf("DEBUG: Received Password via UART: %s\n", input);
    uint8_t incorrectAttempts = 0;
    uint8_t input[10];

    for (;;) {
        const uint8_t command = COMM_ReceiveCommand();
        // Already executed command

        switch (command) {
        case CMD_SEND_PASSWORD:{
                COMM_ReceiveMessage(input);
                bool volatile isCorrect = compare_Passwords(input);

                if (isCorrect == true) {
                    ResetAttempts(&incorrectAttempts);
                    COMM_SendCommand(CMD_PASSWORD_CORRECT);
                    toggle_LED(1 << 3);
                } else {
                    IncrementAttempts(&incorrectAttempts);
                    COMM_SendCommand(CMD_PASSWORD_WRONG);
                    toggle_LED(1 << 1);
                }
                WaitForAck();
                break;
        }
            case CMD_DOOR_UNLOCK: 
                COMM_SendCommand(CMD_ACK);
              volatile int seconds = get_AutoLockTimeout();
            // get_AutoLockTimeout() ######ADD THIS IN ATART MOTOR() 
                start_Motor(seconds);
                
                break;
        case CMD_CHANGE_PASSWORD:{
                COMM_ReceiveMessage(input);
                bool flag = change_Password(input);
                if(flag){
                     COMM_SendCommand(CMD_ACK); //return ack
                     set_init_flag();
                     toggle_LED(1 << 2);
                }
                break;
        }
            case CMD_SET_TIMEOUT:
                COMM_ReceiveMessage(input);
                if (set_AutoLockTimeout(input[0])) {
                    COMM_SendCommand(CMD_SUCCESS);
                } else {
                    // Must be >= 5 && <= 30
                    COMM_SendCommand(CMD_FAIL);
                }
                break;
            default:
                COMM_SendCommand(CMD_UNKNOWN);
                break;
        }
    }
    return 0;
}

// Loop indefinitely until a response is made
void inline WaitForAck(void) {
    // record start time
    uint32_t start = GetTicks();

    while (COMM_ReceiveCommand() != CMD_ACK);
}

void inline IncrementAttempts(uint8_t *attempts) {
    if (*attempts < MAX_ATTEMPTS) {
        ++(*attempts);
    } else {
        Buzzer_Start();
        while (buzzer_State() == 1 ) {
          // Freeze until the buzzer finishes beeping
        }
    }
}

void inline ResetAttempts(uint8_t *attempts) {
    *attempts = 0;
}

// #define USED_BLOCK 1 //used block 1 to store passwords
// #define PASSWORD_OFFSET 0 //used byte offset 0 to store password inside 

// void EEPROM_HW_Init2(void) {
//     SYSCTL_RCGCEEPROM_R |= 0x01; // Enable EEPROM clock
//     for(int i=0; i<100; i++);    // Small delay for clock to stabilize
//     while (EEPROM_EEDONE_R & 0x01); // Wait for done bit

//     // Software reset the EEPROM
//     SYSCTL_SREEPROM_R = 1;
//     SYSCTL_SREEPROM_R = 0;
//     while (EEPROM_EEDONE_R & 0x01); // Wait for done bit
    
// }



// void EEPROM_Write_Word(uint32_t block, uint32_t offset, uint32_t data) {
//     // 1. Set the block and offset
//     EEPROM_EEBLOCK_R = block;
//     EEPROM_EEOFFSET_R = offset;

//     // 2. Write the data
//     EEPROM_EERDWR_R = data;

//     // 3. Wait for the EEPROM to finish writing (EEDONE bit 0 becomes 0)
//     while (EEPROM_EEDONE_R & 0x01);
// }

// int main(void) {
//     EEPROM_HW_Init2();

//     // Store "1234" as an integer. 
//     // In memory, this will look like the hex equivalent of 1234.
//     EEPROM_Write_Word(USED_BLOCK, PASSWORD_OFFSET, 1234);

//     while(1) {
//         // Main loop
//     }
// }

