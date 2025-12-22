# Door Locker Security System

A dual-microcontroller embedded security system for door access control, built on the **TM4C123GH6PM** (Tiva C Series) platform. This project implements a complete security solution with password authentication, EEPROM storage, motor control, and alarm functionality.

## Table of Contents

- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Features](#features)
- [Hardware Components](#hardware-components)
- [Software Architecture](#software-architecture)
- [Project Structure](#project-structure)
- [Communication Protocol](#communication-protocol)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Configuration](#configuration)
- [Development](#development)
- [License](#license)

## Overview

The Door Locker Security System is a comprehensive embedded solution that separates user interface logic from security-critical operations using two microcontrollers:

- **HMI ECU (Human-Machine Interface)**: Handles user interactions through LCD, keypad, and potentiometer
- **Control ECU**: Manages security logic, password storage, motor control, and alarm system

This architecture ensures that security-critical operations remain isolated from the user interface, enhancing system security.

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                         HMI ECU                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │   LCD    │  │  Keypad  │  │   LED    │  │   POT    │     │
│  │ Display  │  │  Input   │  │ Status   │  │ Timeout  │     │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘     │
│                           │                                 │
│                      ┌────▼────┐                            │
│                      │  UART   │                            │
│                      └────┬────┘                            │
└───────────────────────────┼─────────────────────────────────┘
                            │
                    UART Communication
                            │
┌───────────────────────────▼─────────────────────────────────┐
│                      Control ECU                            │
│                      ┌────┬────┐                            │
│                      │  UART   │                            │
│                      └────┬────┘                            │
│                           │                                 │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │  EEPROM  │  │  Motor   │  │  Buzzer  │  │  Timer   │     │
│  │ Password │  │  Door    │  │  Alarm   │  │ Control  │     │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘     │
└─────────────────────────────────────────────────────────────┘
```

## Features

### Security Features
- **Password Authentication**: 5-digit numeric password with secure storage
- **Failed Attempt Protection**: Maximum 3 attempts before lockout
- **Automatic Lockout**: 60-second alarm activation after failed attempts
- **Password Change**: Secure password modification with verification
- **EEPROM Storage**: Non-volatile password storage with initialization flag

### User Interface
- **16x2 LCD Display**: Clear status messages and menu navigation
- **4x4 Matrix Keypad**: Password entry and menu selection
- **LED Indicators**: Visual feedback for system status
- **Potentiometer**: Adjustable auto-lock timeout (5-30 seconds)

### Door Control
- **Motor Control**: Automated door locking/unlocking
- **Configurable Timing**: Customizable unlock duration
- **Auto-Lock**: Automatic door locking after timeout
- **Status Display**: Real-time door operation feedback

### Communication
- **UART Protocol**: Reliable inter-ECU communication
- **Command-Based**: Structured command/response protocol
- **Error Handling**: Acknowledgment and retry mechanisms

## 🔧 Hardware Components

### HMI ECU Peripherals
- **LCD**: 16x2 character display (HD44780 compatible)
- **Keypad**: 4x4 matrix keypad
- **LED**: RGB LED for status indication
- **Potentiometer**: Analog input for timeout adjustment
- **UART**: Serial communication (9600 baud)

### Control ECU Peripherals
- **EEPROM**: Internal TM4C123 EEPROM for password storage
- **DC Motor**: Door lock mechanism control
- **Buzzer**: Alarm/alert system
- **UART**: Serial communication (9600 baud)
- **SysTick Timer**: Timing and delay management

## Software Architecture

### Layered Architecture

```
┌─────────────────────────────────────────┐
│        Application Layer (HMI)          │
│     - State Machine                     │
│     - User Interface Logic              │
│     - Menu Handling                     │
├─────────────────────────────────────────┤
│     Hardware Abstraction Layer (HAL)    │
│     - LCD Driver                        │
│     - Keypad Driver                     │
│     - Communication Interface           │
├─────────────────────────────────────────┤
│   Microcontroller Abstraction (MCAL)    │
│     - UART Driver                       │
│     - GPIO Driver                       │
│     - ADC Driver                        │
│     - Timer Driver                      │
└─────────────────────────────────────────┘
```

### Key Modules

#### HMI ECU
- **`hmi.c/h`**: Main application logic and state machine
- **`lcd`**: LCD display driver
- **`keypad`**: Matrix keypad driver
- **`led`**: LED control driver
- **`pot`**: Potentiometer (ADC) driver

#### Control ECU
- **`ECU_COMM.c`**: Main control logic and command handler
- **`eeprom`**: EEPROM driver for password storage
- **`motor`**: DC motor control driver
- **`buzzer`**: Buzzer/alarm driver
- **`timer`**: SysTick timer utilities
- **`password_init`**: Password initialization helpers

#### Common
- **`comm_interface`**: UART communication abstraction
- **`uart`**: UART hardware driver

## Project Structure

```
Door-Locker-Security-System/
├── Common/                      # Shared code between ECUs
│   ├── HAL/
│   │   └── comm_interface.c/h   # Communication abstraction
│   └── MCAL/
│       ├── uart.c/h             # UART driver
│       └── tm4c123gh6pm.h       # MCU register definitions
│
├── HMI_ECU/                     # Human-Machine Interface ECU
│   ├── App/
│   │   └── hmi.c/h              # Main HMI application
│   ├── HAL/
│   │   ├── keypad/              # Keypad driver
│   │   ├── lcd/                 # LCD driver
│   │   ├── led/                 # LED driver
│   │   └── pot/                 # Potentiometer driver
│   ├── MCAL/                    # MCU-specific drivers
│   └── main.c                   # HMI entry point
│
├── Control_ECU/                 # Control/Security ECU
│   ├── Drivers/
│   │   ├── Eeprom/              # EEPROM driver
│   │   ├── Motor/               # Motor control
│   │   └── Buzzer/              # Buzzer driver
│   ├── Helpers/
│   │   ├── password_init.c/h    # Password utilities
│   │   └── timer.c/h            # Timer utilities
│   ├── Tests/                   # Unit tests
│   └── ECU_COMM.c               # Control ECU entry point
│
├── External/                    # External libraries
├── Drivers_Test_Project/        # Driver testing
├── CMakeLists.txt               # CMake build configuration
├── Project_Workbench.eww        # IAR workspace
└── README.md                    # This file
```

## 🔌 Communication Protocol

### Command Codes

| Command                | Code | Description                    |
| ---------------------- | ---- | ------------------------------ |
| `CMD_READY`            | 0x10 | ECU ready signal               |
| `CMD_SEND_PASSWORD`    | 0x11 | Send password for verification |
| `CMD_PASSWORD_CORRECT` | 0x12 | Password verified successfully |
| `CMD_PASSWORD_WRONG`   | 0x13 | Password verification failed   |
| `CMD_CHANGE_PASSWORD`  | 0x14 | Change password request        |
| `CMD_DOOR_UNLOCK`      | 0x15 | Unlock door command            |
| `CMD_DOOR_LOCK`        | 0x16 | Lock door command              |
| `CMD_SET_TIMEOUT`      | 0x17 | Set auto-lock timeout          |
| `CMD_SUCCESS`          | 0x18 | Operation successful           |
| `CMD_FAIL`             | 0x19 | Operation failed               |
| `CMD_ALARM`            | 0x1A | Trigger alarm                  |
| `CMD_ACK`              | 0x1B | Acknowledgment                 |
| `CMD_INIT`             | 0x1C | Initialize password            |

### Message Format

```
┌──────────┬──────────────┬──────────┐
│ Command  │   Payload    │   End    │
│ (1 byte) │ (variable)   │  ('\n')  │
└──────────┴──────────────┴──────────┘
```

### Communication Flow Example

**Password Verification:**
```
HMI → Control: CMD_SEND_PASSWORD
HMI → Control: "12345\n"
Control → HMI: CMD_PASSWORD_CORRECT / CMD_PASSWORD_WRONG
HMI → Control: CMD_ACK
```

## Getting Started

### Prerequisites

- **Hardware**: 2x TM4C123GH6PM LaunchPad boards
- **IDE**: IAR Embedded Workbench for ARM (or compatible)
- **Debugger**: TI Stellaris ICDI debugger
- **Components**: LCD, Keypad, Motor, Buzzer, LEDs, Potentiometer

### Building the Project

#### Using IAR Embedded Workbench

1. Open `Project_Workbench.eww` in IAR
2. Select the target ECU project (HMI_ECU or Control_ECU)
3. Build → Make (F7)
4. Flash to the respective board

### Hardware Setup

1. **Connect UART**: Cross-connect TX/RX between ECUs
   - HMI ECU TX → Control ECU RX
   - HMI ECU RX → Control ECU TX
   - Common GND

2. **HMI ECU Connections**:
   - LCD to GPIO pins (see `lcd.h` for pinout)
   - Keypad to GPIO pins (see `keypad.h` for pinout)
   - LED to Port F (PF1-Red, PF2-Blue, PF3-Green)
   - Potentiometer to ADC pin (PE3)

3. **Control ECU Connections**:
   - Motor driver to GPIO pins
   - Buzzer to GPIO pin
   - EEPROM is internal (no external connection)

4. **Power both boards** via USB or external 5V supply

##  Usage

### First-Time Setup

1. Power on both ECUs
2. System will prompt: **"Set Password:"**
3. Enter a 5-digit password using keypad
4. Confirm password when prompted
5. System displays: **"Password Saved!"**

### Main Menu

```
A: Open Door
B: Change Password
C: Set Timeout
```

### Opening the Door

1. Press **'A'** from main menu
2. Enter password
3. If correct:
   - Display shows "Unlocking..."
   - Motor rotates to unlock
   - Display shows "Door Open" with countdown
   - Motor rotates to lock after timeout
4. If incorrect:
   - Display shows "Wrong Password"
   - 3 attempts allowed
   - After 3 failures: 60-second lockout with alarm

### Changing Password

1. Press **'B'** from main menu
2. Enter current password
3. Enter new password
4. Confirm new password
5. System saves and confirms

### Setting Auto-Lock Timeout

1. Press **'C'** from main menu
2. Adjust potentiometer (5-30 seconds displayed)
3. Press **'#'** to confirm
4. Enter password to save
5. New timeout is applied

## Configuration

### Password Settings
```c
#define PASSWORD_LENGTH         5      // 5-digit password
#define MAX_PASSWORD_ATTEMPTS   3      // Max failed attempts
```

### Timeout Settings
```c
#define MIN_TIMEOUT_SEC         5      // Minimum auto-lock time
#define MAX_TIMEOUT_SEC         30     // Maximum auto-lock time
#define DEFAULT_TIMEOUT_SEC     10     // Default timeout
```

### Door Timing
```c
#define DOOR_UNLOCK_TIME        15     // Unlock duration (seconds)
#define DOOR_LOCK_TIME          15     // Lock duration (seconds)
#define LOCKOUT_DURATION_SEC    60     // Lockout period (seconds)
```

### UART Configuration
```c
#define UART_BAUD_RATE          9600   // Communication speed
```

## Development

### Adding New Features

1. **New Commands**: Add to `COMM_CommandID` enum in `comm_interface.h`
2. **HMI States**: Extend state machine in `hmi.c`
3. **Control Logic**: Add handlers in `ECU_COMM.c`

### Testing

- **Unit Tests**: Located in `Control_ECU/Tests/`
- **Driver Tests**: Located in `Drivers_Test_Project/`

### Debugging

- Use IAR debugger with breakpoints
- LED indicators for visual debugging
- UART output for logging (if enabled)

### Code Style

- Follow embedded C best practices
- Use meaningful variable names
- Comment complex logic
- Keep functions modular and focused

## Contributing

This is an academic project for ASU Fall 2025 Introduction to Embedded Systems course.

## License

This project is developed for educational purposes.

---

**Project Date**: December 2025  
**Platform**: TM4C123GH6PM (Tiva C Series)  
**Course**: Introduction to Embedded Systems - ASU Fall 2025


---

**Built with ❤️ using TI Tiva C Series Microcontrollers**
