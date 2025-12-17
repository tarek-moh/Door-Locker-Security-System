#include "../../../External/unity.h"
#include "eeprom_unit_test.h"

int main(void) {
    UNITY_BEGIN();  // Initialize Unity

    /* ---------- PASSWORD TESTS ---------- */
    RUN_TEST(test_compare_passwords_match);
    RUN_TEST(test_compare_passwords_mismatch);
    RUN_TEST(test_change_password_success);
    RUN_TEST(test_change_password_null_pointer);
    RUN_TEST(test_change_password_write_fail);

    /* ---------- AUTO LOCK TESTS ---------- */
    RUN_TEST(test_get_autolock_timeout);
    RUN_TEST(test_set_autolock_valid_range);
    RUN_TEST(test_set_autolock_below_min);
    RUN_TEST(test_set_autolock_above_max);
    RUN_TEST(test_set_autolock_preserves_upper_bytes);

    return UNITY_END();  // Print summary
}