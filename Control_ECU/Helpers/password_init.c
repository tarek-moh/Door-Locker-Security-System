#include "password_init.h"

bool is_password_init(void)
{
    return ((EEPROM_HW_ReadWord(1, 2) & 1) == 1);
}

void set_init_flag(void)
{
    EEPROM_HW_WriteWord(1, 2, 1);
}