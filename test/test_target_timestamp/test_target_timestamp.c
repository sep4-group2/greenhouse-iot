#ifdef TARGET_TEST
#include <avr/io.h>
#include <util/delay.h>
#endif

#include "unity.h"
#include "timestamp.h"

void setUp(void) {
    timestamp_init();
}

void tearDown(void) {}

void test_timestamp_set_and_get_time(void) {
    timestamp_set(12, 34, 56, 0, 0, 0);
    uint8_t h, m, s;
    timestamp_get(&h, &m, &s);
    TEST_ASSERT_EQUAL_UINT8(12, h);
    TEST_ASSERT_EQUAL_UINT8(34, m);
    TEST_ASSERT_EQUAL_UINT8(56, s);
}

void test_timestamp_set_and_get_date(void) {
    timestamp_set(0, 0, 0, 25, 12, 2025);
    uint8_t d, mo;
    uint16_t y;
    timestamp_get_date(&d, &mo, &y);
    TEST_ASSERT_EQUAL_UINT8(25, d);
    TEST_ASSERT_EQUAL_UINT8(12, mo);
    TEST_ASSERT_EQUAL_UINT16(2025, y);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_timestamp_set_and_get_time);
    RUN_TEST(test_timestamp_set_and_get_date);
    return UNITY_END();
}
