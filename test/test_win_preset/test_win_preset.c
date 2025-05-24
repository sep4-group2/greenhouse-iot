#include "unity.h"
#include "preset.h"

void setUp(void) {}
void tearDown(void) {}

void test_preset_initializes_with_manual_values(void) {
    // Arrange & Act
    preset_t p = preset_init();

    // Assert
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL(ACTION_MANUAL, preset_get_watering_method(p));
    TEST_ASSERT_EQUAL(ACTION_MANUAL, preset_get_lighting_method(p));
    TEST_ASSERT_EQUAL(ACTION_MANUAL, preset_get_fertilizer_method(p));
    TEST_ASSERT_EQUAL(0, preset_get_min_soil_humidity(p));
    TEST_ASSERT_EQUAL(0, preset_get_max_soil_humidity(p));
    TEST_ASSERT_EQUAL(0, preset_get_light_hours(p));

    preset_destroy(p);
}

void test_preset_setters_and_getters_work_correctly(void) {
    // Arrange
    preset_t p = preset_init();

    // Act
    preset_set_watering_method(p, ACTION_AUTOMATED);
    preset_set_lighting_method(p, ACTION_AUTOMATED);
    preset_set_fertilizer_method(p, ACTION_AUTOMATED);
    preset_set_min_soil_humidity(p, 25);
    preset_set_max_soil_humidity(p, 75);
    preset_set_light_hours(p, 12);

    // Assert
    TEST_ASSERT_EQUAL(ACTION_AUTOMATED, preset_get_watering_method(p));
    TEST_ASSERT_EQUAL(ACTION_AUTOMATED, preset_get_lighting_method(p));
    TEST_ASSERT_EQUAL(ACTION_AUTOMATED, preset_get_fertilizer_method(p));
    TEST_ASSERT_EQUAL(25, preset_get_min_soil_humidity(p));
    TEST_ASSERT_EQUAL(75, preset_get_max_soil_humidity(p));
    TEST_ASSERT_EQUAL(12, preset_get_light_hours(p));

    preset_destroy(p);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_preset_initializes_with_manual_values);
    RUN_TEST(test_preset_setters_and_getters_work_correctly);
    return UNITY_END();
}
