#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H
#include <stdint.h>

int compare_passwords(const uint8_t *password);

void change_password(const uint8_t *new_password);

#endif //EEPROM_DRIVER_H