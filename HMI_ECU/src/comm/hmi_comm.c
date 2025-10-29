#include "hmi_comm.h"
#include "comm_interface.h"
#include "src/door/door_driver.h"
#include "src/eeprom/eeprom_driver.h"

int main(void) {
    // Initialize the communication path
    COMM_Init();
    // Send a command to the Control_ECU that the HMI_ECU is reading for communication
    COMM_SendCommand(CMD_READY);
    // Loop indefinitely until an acknowledgement of the connection is received
    while (COMM_ReceiveCommand() != CMD_ACK) { }

    for (;;) {
        uint8_t command = COMM_ReceiveCommand();

        switch (command) {
            case CMD_COMPARE_PASSWORD:
                uint8_t input[10];
                COMM_ReceiveMessage(input);
                int bool = compare_passwords(input);

                if (bool == 1) {
                    unlock_door();
                } else {
                    // Act accordingly
                }
                break;
            case CMD_DOOR_LOCK:
                // Lock the door
                break;
            case CMD_DOOR_UNLOCK:
                // Unlock the door
                break;
            case CMD_CHANGE_PASSWORD:
                // Change the password
                break;
        }
    }

    return 0;
}
