# Overview
this document provide a brief documentation documentation of the Comm_interface, providing the list of commands that both Control and HMI ECUs will use. Please refer to it if you find the header files confusing, or ask me directly :) 

---
# System Commands Codes
this is enumerated in the comm_inteface.h file. No need to use their Hex values you can directly use the cmd name.
| Command Name         | Hex Value | Description                     |
|----------------------|-----------|----------------------------------|
| CMD_READY            | 0x10      | Indicates readiness to start     |
| CMD_SEND_PASSWORD    | 0x11      | HMI sends password string        |
| CMD_PASSWORD_CORRECT | 0x12      | Password match confirmed         |
| CMD_PASSWORD_WRONG   | 0x13      | Password mismatch detected       |
| CMD_CHANGE_PASSWORD  | 0x14      | Initiate password change process |
| CMD_DOOR_UNLOCK      | 0x15      | Request door unlock              |
| CMD_DOOR_LOCK        | 0x16      | Request door lock                |
| CMD_ALARM            | 0x17      | Trigger security alarm           |
| CMD_ACK              | 0x18      | Acknowledgment message           |

---

# Example Usecases of the Communication Interface

## System Startup Handshake
| Step | Sender      | Action                   |
| ---- | ----------- | ------------------------ |
| 1    | HMI_ECU     | Send `CMD_READY`         |
| 2    | Control_ECU | Reply with `CMD_ACK`     |
| 3    | Both        | Confirm UART link active |


## Password Entry
| Step | Sender      | Action                                              |
| ---- | ----------- | --------------------------------------------------- |
| 1    | HMI_ECU     | Send `CMD_SEND_PASSWORD`                            |
| 2    | HMI_ECU     | Send password string                                |
| 3    | Control_ECU | Compare with EEPROM                                 |
| 4    | Control_ECU | Send `CMD_PASSWORD_CORRECT` or `CMD_PASSWORD_WRONG` |

## Door Unlock / Lock Sequence
| Step | Sender      | Action                    |
| ---- | ----------- | ------------------------- |
| 1    | HMI_ECU     | Send `CMD_DOOR_UNLOCK`    |
| 2    | Control_ECU | Rotate motor (open door)  |
| 3    | Control_ECU | Send `CMD_ACK`            |
| 4    | HMI_ECU     | Show “Door is Unlocking…” |

## Change Password Procedure
| Step | Sender      | Action                     |
| ---- | ----------- | -------------------------- |
| 1    | HMI_ECU     | Send `CMD_CHANGE_PASSWORD` |
| 2    | HMI_ECU     | Send new password string   |
| 3    | Control_ECU | Update EEPROM              |
| 4    | Control_ECU | Send `CMD_ACK`             |

### and many more...

# Example Usage (HMI_ECU Side)
```
#include "comm_interface.h"
int main(void)
{
    COMM_Init();
    COMM_SendByte(CMD_READY);
    if (COMM_ReceiveByte() == CMD_ACK)
    {
        COMM_SendByte(CMD_SEND_PASSWORD);
        COMM_SendMessage((uint8_t*)"1234");
    }
}
```
# Example Usage (Control_ECU Side)
```
#include "comm_interface.h"
int main(void)
{
    COMM_Init();
    if (COMM_ReceiveByte() == CMD_READY)
    {
        COMM_SendByte(CMD_ACK);
    }

    uint8_t command = COMM_ReceiveByte();
    if (command == CMD_SEND_PASSWORD)
    {
        uint8_t password[10];
        COMM_ReceiveMessage(password);
        // Compare password and reply accordingly
    }
}
```

---
**Note**
The communication protocol can be easily extended by adding new `CMD_` codes to the `COMM_CommandID` enum and handling them on both ECUs.  
Keep all changes synchronized between both sides to maintain compatibility.
