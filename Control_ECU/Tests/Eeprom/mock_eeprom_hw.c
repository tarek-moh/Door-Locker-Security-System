#include "../../Drivers/Eeprom/eeprom_hw.h"

static uint32_t fake_eeprom[4][16];
static bool force_write_fail;

void EEPROM_HW_Init(void) {}

uint32_t EEPROM_HW_ReadWord(uint32_t block, uint32_t offset) {
    return fake_eeprom[block][offset];
}

bool EEPROM_HW_WriteWord(uint32_t block, uint32_t offset, uint32_t value) {
    if (force_write_fail) return false;
    fake_eeprom[block][offset] = value;
    return true;
}

uint32_t EEPROM_HW_GetStatus(void) {
    return 0;
}

/* helpers for tests */
void mock_eeprom_clear(void) {
    for (int b = 0; b < 4; b++)
        for (int o = 0; o < 16; o++)
            fake_eeprom[b][o] = 0;
}

void mock_eeprom_set(uint32_t block, uint32_t offset, uint32_t value) {
    fake_eeprom[block][offset] = value;
}

void mock_eeprom_force_fail(bool enable) {
    force_write_fail = enable;
}