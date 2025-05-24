#include "unity.h"
#include "clock.h"

void setUp(void) {}
void tearDown(void) {}

void test_clock_init_sets_all_fields(void) {
    // Arrange
    Clock c;

    // Act
    clock_init(&c, 2025, 5, 20, 12, 34, 56);

    // Assert
    TEST_ASSERT_EQUAL_UINT16(2025, c.year);
    TEST_ASSERT_EQUAL_UINT8(5, c.month);
    TEST_ASSERT_EQUAL_UINT8(20, c.day);
    TEST_ASSERT_EQUAL_UINT8(12, c.hour);
    TEST_ASSERT_EQUAL_UINT8(34, c.minute);
    TEST_ASSERT_EQUAL_UINT8(56, c.second);
}

void test_clock_tick_increments_seconds(void) {
    Clock c = {2025, 5, 20, 12, 34, 56};
    clock_tick(&c);
    TEST_ASSERT_EQUAL_UINT8(57, c.second);
}

void test_clock_tick_rolls_over_seconds_to_minutes(void) {
    Clock c = {2025, 5, 20, 12, 34, 59};
    clock_tick(&c);
    TEST_ASSERT_EQUAL_UINT8(0, c.second);
    TEST_ASSERT_EQUAL_UINT8(35, c.minute);
}

void test_clock_tick_rolls_over_minutes_to_hour(void) {
    Clock c = {2025, 5, 20, 12, 59, 59};
    clock_tick(&c);
    TEST_ASSERT_EQUAL_UINT8(0, c.second);
    TEST_ASSERT_EQUAL_UINT8(0, c.minute);
    TEST_ASSERT_EQUAL_UINT8(13, c.hour);
}

void test_clock_tick_rolls_over_hours_to_next_day(void) {
    Clock c = {2025, 5, 20, 23, 59, 59};
    clock_tick(&c);
    TEST_ASSERT_EQUAL_UINT8(0, c.second);
    TEST_ASSERT_EQUAL_UINT8(0, c.minute);
    TEST_ASSERT_EQUAL_UINT8(0, c.hour);
    TEST_ASSERT_EQUAL_UINT8(21, c.day);
}

void test_clock_tick_rolls_over_day_to_month(void) {
    Clock c = {2025, 1, 31, 23, 59, 59};
    clock_tick(&c);
    TEST_ASSERT_EQUAL_UINT8(1, c.day);
    TEST_ASSERT_EQUAL_UINT8(2, c.month);
}

void test_clock_tick_rolls_over_month_to_year(void) {
    Clock c = {2025, 12, 31, 23, 59, 59};
    clock_tick(&c);
    TEST_ASSERT_EQUAL_UINT8(1, c.day);
    TEST_ASSERT_EQUAL_UINT8(1, c.month);
    TEST_ASSERT_EQUAL_UINT16(2026, c.year);
}

void test_clock_to_string_formats_correctly(void) {
    Clock c = {2025, 5, 20, 9, 4, 7};
    char buffer[32];
    clock_to_string(&c, buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL_STRING("2025-05-20T09:04:07Z", buffer);
}

void test_clock_update_task_advances_global_clock(void) {
    extern Clock global_clock;
    global_clock = (Clock){2025, 5, 20, 10, 0, 0};
    clock_update_task();
    TEST_ASSERT_EQUAL_UINT8(1, global_clock.second);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_clock_init_sets_all_fields);
    RUN_TEST(test_clock_tick_increments_seconds);
    RUN_TEST(test_clock_tick_rolls_over_seconds_to_minutes);
    RUN_TEST(test_clock_tick_rolls_over_minutes_to_hour);
    RUN_TEST(test_clock_tick_rolls_over_hours_to_next_day);
    RUN_TEST(test_clock_tick_rolls_over_day_to_month);
    RUN_TEST(test_clock_tick_rolls_over_month_to_year);
    RUN_TEST(test_clock_to_string_formats_correctly);
    RUN_TEST(test_clock_update_task_advances_global_clock);
    return UNITY_END();
}
