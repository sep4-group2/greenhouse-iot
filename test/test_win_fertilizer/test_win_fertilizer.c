#include "unity.h"
#include "fertilizer.h"
#define FFF_GLOBALS
#include "../fff.h"

FAKE_VOID_FUNC(servo, int);

void setUp(void) {
    RESET_FAKE(servo);
}

void tearDown(void) {}

void test_fertilizer_trigger_should_call_servo_with_90_and_0(void) {
    fertilizer_trigger();

    TEST_ASSERT_EQUAL(2, servo_fake.call_count);
    TEST_ASSERT_EQUAL(90, servo_fake.arg0_history[0]);
    TEST_ASSERT_EQUAL(0, servo_fake.arg0_history[1]);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_fertilizer_trigger_should_call_servo_with_90_and_0);
    return UNITY_END();
}
