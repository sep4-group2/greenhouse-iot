#include "unity.h"
#include "lightbulb.h"
#include <avr/io.h>

void setUp(void) {
    DDRA = 0x00;
    PORTA = 0xFF;
}

void tearDown(void) {}

void test_lightbulb_init_should_set_PA7_as_output_and_low(void) {
    // Act
    lightbulb_init();
    // Assert
    TEST_ASSERT_BIT_HIGH(PA7, DDRA);
    TEST_ASSERT_BIT_LOW(PA7, PORTA);
}

void test_lightbulb_on_should_set_PA7_high(void) {
    // Arrange
    lightbulb_init();
    // Act
    lightbulb_on();
    // Assert
    TEST_ASSERT_BIT_HIGH(PA7, PORTA);
}

void test_lightbulb_off_should_set_PA7_low(void) {
    // Arrange
    lightbulb_init();
    lightbulb_on();
    // Act
    lightbulb_off();
    // Assert
    TEST_ASSERT_BIT_LOW(PA7, PORTA);
}

void test_lightbulb_toggle_should_invert_PA7(void) {
    // Arrange
    lightbulb_init();
    lightbulb_off();
    // Act & Assert
    lightbulb_toggle();
    TEST_ASSERT_BIT_HIGH(PA7, PORTA);
    lightbulb_toggle();
    TEST_ASSERT_BIT_LOW(PA7, PORTA);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_lightbulb_init_should_set_PA7_as_output_and_low);
    RUN_TEST(test_lightbulb_on_should_set_PA7_high);
    RUN_TEST(test_lightbulb_off_should_set_PA7_low);
    RUN_TEST(test_lightbulb_toggle_should_invert_PA7);
    return UNITY_END();
}
