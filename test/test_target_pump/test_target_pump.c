#include "unity.h"
#include "pump.h"
#include <avr/io.h>

void setUp(void) {
    DDRC = 0x00;
    PORTC = 0xFF;
}

void tearDown(void) {}

void test_pump_init_should_set_PC7_as_output_and_low() {
    pump_init();
    TEST_ASSERT_TRUE((DDRC & (1 << PC7)) != 0);
    TEST_ASSERT_TRUE((PORTC & (1 << PC7)) == 0);
}

void test_pump_on_should_set_PC7_high() {
    pump_init();
    pump_on();
    TEST_ASSERT_TRUE(PORTC & (1 << PC7));
}

void test_pump_off_should_set_PC7_low() {
    pump_init();
    pump_on();
    pump_off();
    TEST_ASSERT_FALSE(PORTC & (1 << PC7));
}

void test_pump_toggle_should_invert_PC7() {
    pump_init();
    pump_off();
    pump_toggle();
    TEST_ASSERT_TRUE(PORTC & (1 << PC7));
    pump_toggle();
    TEST_ASSERT_FALSE(PORTC & (1 << PC7));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pump_init_should_set_PC7_as_output_and_low);
    RUN_TEST(test_pump_on_should_set_PC7_high);
    RUN_TEST(test_pump_off_should_set_PC7_low);
    RUN_TEST(test_pump_toggle_should_invert_PC7);
    return UNITY_END();
}
