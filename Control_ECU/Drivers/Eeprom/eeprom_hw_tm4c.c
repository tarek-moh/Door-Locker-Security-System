#include "eeprom_hw.h"
#include "../../../Common/MCAL/tm4c123gh6pm.h"

void EEPROM_HW_Init(void) {
    SYSCTL_RCGCEEPROM_R |= 0x01;
    while (EEPROM_EEDONE_R & 1);

    SYSCTL_SREEPROM_R = 1;
    SYSCTL_SREEPROM_R = 0;
    while (EEPROM_EEDONE_R & 1);
}

uint32_t EEPROM_HW_ReadWord(uint32_t block, uint32_t offset) {
    EEPROM_EEBLOCK_R  = block;
    EEPROM_EEOFFSET_R = offset;
    return EEPROM_EERDWR_R;
}

bool EEPROM_HW_WriteWord(uint32_t block, uint32_t offset, uint32_t value) {
    EEPROM_EEBLOCK_R  = block;
    EEPROM_EEOFFSET_R = offset;
    EEPROM_EERDWR_R   = value;

    while (EEPROM_EEDONE_R & 1);

    if (EEPROM_EEDONE_R &
        (EEPROM_EEDONE_WKCOPY |
         EEPROM_EEDONE_WKERASE |
         EEPROM_EEDONE_NOPERM)) {
        return false;
         }

    return true;
}

uint32_t EEPROM_HW_GetStatus(void) {
    return EEPROM_EEDONE_R;
}