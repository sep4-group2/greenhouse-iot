#include "unity.h"
#include "pump.h"
#ifndef WINDOWS_TEST
#include <avr/io.h>
#include <util/delay.h>
void setUp(void) {
    DDRC = 0x00;
    PORTC = 0xFF;
}

void tearDown(void) {}

void test_pump_init_should_set_PC7_as_output_and_low(void) {
    // Act
    pump_init();
    // Assert
    TEST_ASSERT_BIT_HIGH(PC7, DDRC);
    TEST_ASSERT_BIT_LOW(PC7, PORTC);
}

void test_pump_on_should_set_PC7_high(void) {
    // Arrange
    pump_init();
    // Act
    pump_on();
    // Assert
    TEST_ASSERT_BIT_HIGH(PC7, PORTC);
}

void test_pump_off_should_set_PC7_low(void) {
    // Arrange
    pump_init();
    pump_on();
    // Act
    pump_off();
    // Assert
    TEST_ASSERT_BIT_LOW(PC7, PORTC);
}

void test_pump_toggle_should_invert_PC7(void) {
    // Arrange
    pump_init();
    pump_off();
    // Act & Assert
    pump_toggle();
    TEST_ASSERT_BIT_HIGH(PC7, PORTC);
    pump_toggle();
    TEST_ASSERT_BIT_LOW(PC7, PORTC);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pump_init_should_set_PC7_as_output_and_low);
    RUN_TEST(test_pump_on_should_set_PC7_high);
    RUN_TEST(test_pump_off_should_set_PC7_low);
    RUN_TEST(test_pump_toggle_should_invert_PC7);
    return UNITY_END();
}
#endif