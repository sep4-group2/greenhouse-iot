#include "unity.h"
#include "servo.h"

#ifndef WINDOWS_TEST
#include <avr/io.h>
#include <util/delay.h>

void setUp(void) {
    DDRE = 0;
    PORTE = 0;
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
}

void tearDown(void) {}

void test_servo_should_set_DDRE_bit_for_output(void) {
    // Act
    servo(90);
    // Assert
    TEST_ASSERT_TRUE(DDRE & (1 << PE3));
}

void test_servo_should_set_and_clear_SERVO_pin(void) {
    // Arrange
    PORT_SERVO &= ~(1 << P_SERVO);
    // Act
    servo(90);
    // Assert
    TEST_ASSERT_FALSE(PORT_SERVO & (1 << P_SERVO)); // ends LOW
}

void test_servo_should_clip_angle_above_180(void) {
    // Act
    servo(250);
    // Assert
    TEST_ASSERT_TRUE(DDRE & (1 << PE3));
}

void test_servo_should_clip_angle_below_0(void) {
    // Act
    servo(-20);
    // Assert
    TEST_ASSERT_TRUE(DDRE & (1 << PE3));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_servo_should_set_DDRE_bit_for_output);
    RUN_TEST(test_servo_should_set_and_clear_SERVO_pin);
    RUN_TEST(test_servo_should_clip_angle_above_180);
    RUN_TEST(test_servo_should_clip_angle_below_0);
    return UNITY_END();
}

#endif