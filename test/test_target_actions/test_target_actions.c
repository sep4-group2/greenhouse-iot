#include "unity.h"
#include <stdbool.h>
#include <stdint.h>

void notification_send(char *action, bool status) {}

#include "actions.h"

bool leds_mock[5];
bool pump_state;
bool light_state;

void leds_turnOn(uint8_t pin) { leds_mock[pin] = true; }
void leds_turnOff(uint8_t pin) { leds_mock[pin] = false; }
void leds_toggle(uint8_t pin) { leds_mock[pin] = !leds_mock[pin]; }
bool leds_isOn(uint8_t pin) { return leds_mock[pin]; }

void pump_on() { pump_state = true; }
void pump_off() { pump_state = false; }
bool pump_is_on() { return pump_state; }

void lightbulb_on() { light_state = true; }
void lightbulb_off() { light_state = false; }
void lightbulb_toggle() { light_state = !light_state; }
bool lightbulb_is_on() { return light_state; }

void periodic_task_init_b(void (*callback)(void), uint32_t ms) {}

void fertilizer_trigger() {}

void setUp(void) {
    for (int i = 0; i < 5; i++) leds_mock[i] = false;
    pump_state = false;
    light_state = false;
}

void tearDown(void) {}

void test_actions_pump(void) {
    actions_pump();
    TEST_ASSERT_TRUE(leds_isOn(4));
    TEST_ASSERT_TRUE(pump_is_on());
}

void test_actions_light_toggle(void) {
    bool prev = lightbulb_is_on();
    actions_light_toggle();
    TEST_ASSERT_EQUAL(!prev, lightbulb_is_on());
}

void test_actions_light_on(void) {
    actions_light_on();
    TEST_ASSERT_TRUE(lightbulb_is_on());
    TEST_ASSERT_TRUE(leds_isOn(2));
}

void test_actions_light_off(void) {
    actions_light_off();
    TEST_ASSERT_FALSE(lightbulb_is_on());
    TEST_ASSERT_FALSE(leds_isOn(2));
}

void test_actions_fertilizer(void) {
    actions_fertilizer();
    TEST_PASS();
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

