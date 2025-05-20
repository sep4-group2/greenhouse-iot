#include "unity.h"
#include "notification.h"
#include "fff.h"

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(mqtt_publish, char*, char*, int, int, int);

char mac_address[18] = "AA:BB:CC:DD:EE:FF";

void setUp(void) {
    RESET_FAKE(mqtt_publish);
    FFF_RESET_HISTORY();
}

void tearDown(void) {}

void test_notification_send_should_publish_correct_payload_when_status_true(void) {
    char *expected_topic = "greenhouse/action";
    char *expected_payload_part = "\"Command\":\"Lighting\",\"Status\":true";

    notification_send("Lighting", true);

    TEST_ASSERT_EQUAL_STRING(expected_topic, mqtt_publish_arg0_val);
    TEST_ASSERT_NOT_NULL(mqtt_publish_arg1_val);
    TEST_ASSERT_NOT_NULL(strstr(mqtt_publish_arg1_val, expected_payload_part));
}

void test_notification_send_should_publish_correct_payload_when_status_false(void) {
    notification_send("Watering", false);

    TEST_ASSERT_EQUAL_STRING("greenhouse/action", mqtt_publish_arg0_val);
    TEST_ASSERT_NOT_NULL(mqtt_publish_arg1_val);
    TEST_ASSERT_NOT_NULL(strstr(mqtt_publish_arg1_val, "\"Command\":\"Watering\",\"Status\":false"));
}
