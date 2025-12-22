#include "../../Common/MCAL/tm4c123gh6pm.h"
#include "../Drivers/Eeprom/eeprom_hw.h"


// checks eeprom (is initialized) flag and returns the password state
bool is_password_init(void);

void set_init_flag(void);