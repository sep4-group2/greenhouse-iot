#include "unity.h"
#include "actions.h"
#include "leds.h"
#include "pump.h"
#include "lightbulb.h"
#include "fertilizer.h"
#include "periodic_task.h"
#include <stdbool.h>

char mac_address[18] = "00:11:22:33:44:55";


void setUp(void) {
    pump_off();
    lightbulb_off();
    leds_turnOff(2);
    leds_turnOff(4);
}

void tearDown(void) {}

void test_pump_action_starts_pump_and_led(void) {
    actions_pump();
  
}

void test_light_on_action_activates_led_and_bulb(void) {
    actions_light_on();
}

void test_light_off_action_deactivates_led_and_bulb(void) {
    actions_light_on();
    actions_light_off();
}

void test_light_toggle_action_toggles_state(void) {
    actions_light_off();
    actions_light_toggle();
    actions_light_toggle();
}

void test_fertilizer_action_triggers_servo(void) {
    actions_fertilizer();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pump_action_starts_pump_and_led);
    RUN_TEST(test_light_on_action_activates_led_and_bulb);
    RUN_TEST(test_light_off_action_deactivates_led_and_bulb);
    RUN_TEST(test_light_toggle_action_toggles_state);
    RUN_TEST(test_fertilizer_action_triggers_servo);
    return UNITY_END();
}
