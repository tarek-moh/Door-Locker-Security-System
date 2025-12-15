# 🔊 Buzzer Driver 

**Component:** `buzzer.c` / `buzzer.h`  

---

##  Overview

This module provides **audible feedback** for the system.

Unlike a simple ON/OFF buzzer control, this driver implements a  
**Non-Blocking Alarm Pattern**.

When triggered, it plays a predefined **3-beep sequence**  
(**Short → Medium → Long**) to signal a security event such as:
- Door closing
- Alarm triggered
- Multiple wrong password attempts

The timing is handled entirely using **hardware timers**, allowing the CPU
to continue executing other tasks.

---

##  Resource Usage

This driver **claims the following MCU resources**.  
Do **NOT** use these resources in other modules.

- **Timer0 (32-bit mode)**  
  Used for beep duration and silence intervals
- **Port B (PB0)**  
  Dedicated buzzer output pin

---

##  API Reference

### `void Buzzer_Start(void)`

Triggers the alarm beep sequence.

#### Behavior
1. Initializes GPIO **PB0** and **Timer0**
2. Starts the **first beep immediately**
3. Uses Timer0 interrupts to automatically cycle through the pattern  
   (`ON → OFF → ON → OFF → ...`)
4. Returns immediately  
   *(Sequence runs fully in the background via interrupts)*

---

## 🎼 Beep Pattern (Hardcoded)

The current alarm sequence defined in `buzzer.c` is:

| Beep | ON Duration | OFF Duration |
|-----|------------|--------------|
| Beep 1 | 600 ms | 300 ms |
| Beep 2 | 700 ms | 300 ms |
| Beep 3 | 850 ms | Stop |

---
