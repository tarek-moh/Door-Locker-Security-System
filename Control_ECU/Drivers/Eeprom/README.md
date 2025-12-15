#  EEPROM Driver 

**Component:** `eeprom.c` / `eeprom.h`  
**Hardware:** Tiva C Internal EEPROM (Non-Volatile Memory)

---

##  Overview

This module manages the **persistent storage** of the system.

It stores:
- **Critical security data** (user passwords)
- **User configuration settings** (auto-lock timer)

All data is retained even if the system **resets or loses power**.

The driver abstracts the low-level Tiva C EEPROM complexity  
(blocks, offsets, and `RCGCEEPROM` control) into simple, safe **Get / Set APIs**.

---
##  Memory Map

 **CRITICAL:**  
This driver **exclusively uses Block 1** of the internal EEPROM.  
Do **NOT** use Block 1 in other drivers to avoid **data corruption**.

| Block | Word Offset | Data Stored      | Size   | Description |
|------|------------|------------------|--------|-------------|
| 1    | 0          | Password         | 4 Bytes | 4-digit / 4-character user password |
| 1    | 1          | Lock Timeout     | 1 Byte  | Auto-lock duration (seconds) |

---

##  API Reference

###  Password Management

#### `bool compare_Passwords(const uint8_t *entered_password)`

Compares the entered password with the stored password.

**Parameters**
- `entered_password` – Pointer to a **4-byte array** containing user input

**Returns**
- `true` → Passwords match  
- `false` → Passwords do not match

---
#### `bool change_Password(const uint8_t *new_password)`

Overwrites the stored password with a new one.

**Parameters**
- `new_password` – Pointer to a **4-byte array** containing the new password

**Returns**
- `true` → Write successful  
- `false` → EEPROM hardware error occurred

---

###  Configuration Management

#### `int get_AutoLockTimeout(void)`

Retrieves the stored auto-lock duration.

**Returns**
- Auto-lock time in **seconds** (e.g., `10`)

---

#### `bool set_AutoLockTimeout(const uint8_t lockout_time)`

Updates the auto-lock duration stored in EEPROM.

**Parameters**
- `lockout_time` – New auto-lock duration (seconds)

**Validation**
- Must be between **5 and 30 seconds**
- If invalid, the value is **not saved**

**Returns**
- `true` → Successfully saved  
- `false` → Validation failed or EEPROM error

---

##  Important Notes

###  Data Format
- Password-related functions **strictly expect a 4-byte array**.
- Ensure your **keypad or input driver always provides exactly 4 bytes**.
- Passing fewer or more bytes may cause incorrect comparisons or data corruption.

---
