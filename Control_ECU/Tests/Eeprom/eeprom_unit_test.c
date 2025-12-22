#include "../../../External/unity.h"
#include "../../Drivers/Eeprom/eeprom.h"
#include "../../Drivers/Eeprom/eeprom_hw.h"

/* mock helpers */
void mock_eeprom_clear(void);
void mock_eeprom_set(uint32_t block, uint32_t offset, uint32_t value);
void mock_eeprom_force_fail(bool enable);

#define USED_BLOCK 1
#define PASSWORD_OFFSET 0
#define AUTOLOCKOUT_OFFSET 1

void setUp(void) {
    mock_eeprom_clear();
    mock_eeprom_force_fail(false);
}

void tearDown(void) {}

/* ---------- PASSWORD TESTS ---------- */

void test_compare_passwords_match(void) {
    mock_eeprom_set(USED_BLOCK, PASSWORD_OFFSET, 0x04030201);

    uint8_t pass[4] = {1, 2, 3, 4};
    TEST_ASSERT_TRUE(compare_Passwords(pass));
}

void test_compare_passwords_mismatch(void) {
    mock_eeprom_set(USED_BLOCK, PASSWORD_OFFSET, 0x04030201);

    uint8_t pass[4] = {9, 9, 9, 9};
    TEST_ASSERT_FALSE(compare_Passwords(pass));
}

void test_change_password_success(void) {
    uint8_t new_pass[4] = {5, 6, 7, 8};

    TEST_ASSERT_TRUE(change_Password(new_pass));

    /* verify stored value */
    TEST_ASSERT_TRUE(compare_Passwords(new_pass));
}

void test_change_password_null_pointer(void) {
    TEST_ASSERT_FALSE(change_Password(NULL));
}

void test_change_password_write_fail(void) {
    uint8_t pass[4] = {1, 2, 3, 4};
    mock_eeprom_force_fail(true);

    TEST_ASSERT_FALSE(change_Password(pass));

    mock_eeprom_force_fail(false);
}

/* ---------- AUTO LOCK TESTS ---------- */

void test_get_autolock_timeout(void) {
    mock_eeprom_set(USED_BLOCK, AUTOLOCKOUT_OFFSET, 15);

    TEST_ASSERT_EQUAL_INT(15, get_AutoLockTimeout());
}

void test_set_autolock_valid_range(void) {
    TEST_ASSERT_TRUE(set_AutoLockTimeout(10));
    TEST_ASSERT_EQUAL_INT(10, get_AutoLockTimeout());
}

void test_set_autolock_below_min(void) {
    TEST_ASSERT_FALSE(set_AutoLockTimeout(4));
}

void test_set_autolock_above_max(void) {
    TEST_ASSERT_FALSE(set_AutoLockTimeout(31));
}

void test_set_autolock_preserves_upper_bytes(void) {
    /* preset upper bytes */
    mock_eeprom_set(USED_BLOCK, AUTOLOCKOUT_OFFSET, 0xAABBCC00);

    TEST_ASSERT_TRUE(set_AutoLockTimeout(20));

    uint32_t value = EEPROM_HW_ReadWord(USED_BLOCK, AUTOLOCKOUT_OFFSET);

    TEST_ASSERT_EQUAL_HEX32(0xAABBCC14, value);
}