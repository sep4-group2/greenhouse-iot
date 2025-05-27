#include "unity.h"
#include <stdbool.h>
#include <stdint.h>

#define FFF_GLOBALS
#include "../fff.h"

#include "actions.h"

typedef void (*callback_t)(void);

FAKE_VOID_FUNC(leds_turnOn, uint8_t);
FAKE_VOID_FUNC(leds_turnOff, uint8_t);
FAKE_VOID_FUNC(leds_toggle, uint8_t);

FAKE_VOID_FUNC(pump_on);
FAKE_VOID_FUNC(pump_off);
FAKE_VALUE_FUNC(bool, pump_is_on);

FAKE_VOID_FUNC(lightbulb_on);
FAKE_VOID_FUNC(lightbulb_off);
FAKE_VOID_FUNC(lightbulb_toggle);
FAKE_VALUE_FUNC(bool, lightbulb_is_on);

FAKE_VOID_FUNC(periodic_task_init_b, callback_t, uint32_t);
FAKE_VOID_FUNC(fertilizer_trigger);
FAKE_VOID_FUNC(notification_send, char*, bool);

// Setup/teardown
void setUp(void) {
    FFF_RESET_HISTORY();

    RESET_FAKE(leds_turnOn);
    RESET_FAKE(leds_turnOff);
    RESET_FAKE(leds_toggle);
    RESET_FAKE(pump_on);
    RESET_FAKE(pump_off);
    RESET_FAKE(pump_is_on);
    RESET_FAKE(lightbulb_on);
    RESET_FAKE(lightbulb_off);
    RESET_FAKE(lightbulb_toggle);
    RESET_FAKE(lightbulb_is_on);
    RESET_FAKE(periodic_task_init_b);
    RESET_FAKE(fertilizer_trigger);
    RESET_FAKE(notification_send);

    pump_is_on_fake.return_val = true;
    lightbulb_is_on_fake.return_val = false;
}

void tearDown(void) {}

void test_actions_pump(void) {
    actions_pump();
    TEST_ASSERT_EQUAL(1, leds_turnOn_fake.call_count);
    TEST_ASSERT_EQUAL(1, pump_on_fake.call_count);
    TEST_ASSERT_EQUAL(1, periodic_task_init_b_fake.call_count);
}

void test_actions_light_toggle(void) {
    lightbulb_is_on_fake.return_val = false;
    actions_light_toggle();
    TEST_ASSERT_EQUAL(1, leds_toggle_fake.call_count);
    TEST_ASSERT_EQUAL(1, lightbulb_toggle_fake.call_count);
    TEST_ASSERT_EQUAL(1, notification_send_fake.call_count);
    TEST_ASSERT_EQUAL_STRING("Lighting", notification_send_fake.arg0_val);
    TEST_ASSERT_EQUAL(true, notification_send_fake.arg1_val);
}

void test_actions_light_on(void) {
    actions_light_on();
    TEST_ASSERT_EQUAL(1, leds_turnOn_fake.call_count);
    TEST_ASSERT_EQUAL(1, lightbulb_on_fake.call_count);
    TEST_ASSERT_EQUAL(1, notification_send_fake.call_count);
}

void test_actions_light_off(void) {
    actions_light_off();
    TEST_ASSERT_EQUAL(1, leds_turnOff_fake.call_count);
    TEST_ASSERT_EQUAL(1, lightbulb_off_fake.call_count);
    TEST_ASSERT_EQUAL(1, notification_send_fake.call_count);
}

void test_actions_fertilizer(void) {
    actions_fertilizer();
    TEST_ASSERT_EQUAL(1, fertilizer_trigger_fake.call_count);
    TEST_ASSERT_EQUAL(1, notification_send_fake.call_count);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_actions_pump);
    RUN_TEST(test_actions_light_toggle);
    RUN_TEST(test_actions_light_on);
    RUN_TEST(test_actions_light_off);
    RUN_TEST(test_actions_fertilizer);
    return UNITY_END();
}
