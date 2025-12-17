#ifndef EEPROM_HW_H
#define EEPROM_HW_H

#include <stdint.h>
#include <stdbool.h>

void EEPROM_HW_Init(void);

uint32_t EEPROM_HW_ReadWord(uint32_t block, uint32_t offset);

bool EEPROM_HW_WriteWord(uint32_t block, uint32_t offset, uint32_t value);

uint32_t EEPROM_HW_GetStatus(void);

#endif