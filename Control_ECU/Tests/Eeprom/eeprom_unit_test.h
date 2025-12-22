#ifndef EEPROM_UNIT_TEST_H
#define EEPROM_UNIT_TEST_H

#include <stdint.h>
#include <stdbool.h>

/* Mock helper declarations */
void mock_eeprom_clear(void);
void mock_eeprom_set(uint32_t block, uint32_t offset, uint32_t value);
void mock_eeprom_force_fail(bool enable);

/* Unity test setup/teardown */
void setUp(void);
void tearDown(void);

/* ---------- PASSWORD TESTS ---------- */
void test_compare_passwords_match(void);
void test_compare_passwords_mismatch(void);
void test_change_password_success(void);
void test_change_password_null_pointer(void);
void test_change_password_write_fail(void);

/* ---------- AUTO LOCK TESTS ---------- */
void test_get_autolock_timeout(void);
void test_set_autolock_valid_range(void);
void test_set_autolock_below_min(void);
void test_set_autolock_above_max(void);
void test_set_autolock_preserves_upper_bytes(void);

#endif // EEPROM_UNIT_TEST_H