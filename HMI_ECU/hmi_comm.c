#include "hmi_comm.h"
#include "comm_interface.h"

int main(void) {
    unsigned int password = 1234;

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
                // compare the two passwords
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