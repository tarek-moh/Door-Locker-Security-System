#  DC Motor Driver (L298N)

**Component:** `motor.c` / `motor.h`  
**Hardware:** Tiva C LaunchPad (TM4C123GH6PM) + L298N H-Bridge + DC Motor

---

##  Overview

This module handles the physical actuation of the door mechanism using a DC motor.

It implements a **“Fire and Forget”** logic:
- You command the door to **open**
- The driver manages motor direction
- After a fixed duration, the motor **automatically reverses** to close the door using **hardware timer interrupts**

No continuous polling or manual closing is required.

---

##  Hardware Connections (Pinout)

 **CRITICAL:** Wire the components *exactly* as listed below.  
Swapping **IN1 / IN2** will reverse the open/close logic.

| Tiva Pin | Component Pin | Function            | Logic Level |
|--------|---------------|---------------------|-------------|
| PB2    | L298N IN1     | Motor Input 1       | High = Open (Forward) |
| PB3    | L298N IN2     | Motor Input 2       | High = Close (Reverse) |
| PF3    | Green LED     | Status Indicator    | ON when Unlocked |
| PF1    | Red LED       | Status Indicator    | ON when Locked |
| GND    | L298N GND     | Common Ground       | **Must connect Tiva GND to L298N GND** |

---

##  Resource Usage

This driver **claims the following MCU resources**.  
 Do **not** use these elsewhere in your project.

- **Timer1 (32-bit mode)**  
  Used for the auto-lock countdown
- **Port B (PB2, PB3)**  
  Dedicated to motor control
- **Port F (PF1, PF2, PF3)**  
  Dedicated to status LEDs

---

##  API Reference

### `void start_Motor(int auto_lockoutSeconds)`

The **only function** you need to call.

It initializes the motor system, opens the door, and automatically closes it after a specified time.

#### Parameters
- `auto_lockoutSeconds` *(int)*  
  Number of seconds the door remains open (e.g., `5`, `10`)
#### Behavior
1. Initializes:
   - GPIO Port B
   - GPIO Port F
   - Timer1
2. Drives motor **forward** → Door opens
3. Turns **Green LED (PF3)** ON
4. Starts Timer1 countdown
5. Returns immediately  
   *(Non-blocking for timer, short blocking delay for motor movement)*

---

##  Internal Interrupt

### `Timer1A_Handler`

- **Trigger:** Automatically fires after `auto_lockoutSeconds`
- **Action:**
  - Drives motor **backward** → Door closes
  - Turns **Red LED (PF1)** ON
