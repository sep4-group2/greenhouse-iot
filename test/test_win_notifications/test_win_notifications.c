#include "unity.h"
#define FFF_GLOBALS
#include "../fff.h"

#include <string.h>
#include <stdbool.h>

#include "notification.h"

// --- Setup fakes ---
FAKE_VOID_FUNC(mqtt_publish, const char*, const char*, int, int, int);

// --- Variables simuladas ---
char mac_address[18] = "12:34:56:78:9A:BC";

// --- setUp y tearDown ---
void setUp(void) {
    RESET_FAKE(mqtt_publish);
    FFF_RESET_HISTORY();
}

void tearDown(void) {}

// --- Tests ---
void test_notification_send_true_status(void) {
    notification_send("open_window", true);

    TEST_ASSERT_EQUAL(1, mqtt_publish_fake.call_count);
    TEST_ASSERT_EQUAL_STRING("greenhouse/action", mqtt_publish_fake.arg0_val);

    const char* sent_payload = mqtt_publish_fake.arg1_val;

    TEST_ASSERT_NOT_NULL(strstr(sent_payload, "\"Command\":\"open_window\""));
    TEST_ASSERT_NOT_NULL(strstr(sent_payload, "\"Status\":true"));
    TEST_ASSERT_NOT_NULL(strstr(sent_payload, "\"MacAddress\":\"12:34:56:78:9A:BC\""));
    TEST_ASSERT_NOT_NULL(strstr(sent_payload, "\"Timestamp\":\"0000-00-00T00:00:00Z\""));
}

void test_notification_send_false_status(void) {
    notification_send("close_window", false);

    TEST_ASSERT_EQUAL(1, mqtt_publish_fake.call_count);
    TEST_ASSERT_EQUAL_STRING("greenhouse/action", mqtt_publish_fake.arg0_val);

    const char* sent_payload = mqtt_publish_fake.arg1_val;

    TEST_ASSERT_NOT_NULL(strstr(sent_payload, "\"Command\":\"close_window\""));
    TEST_ASSERT_NOT_NULL(strstr(sent_payload, "\"Status\":false"));
    TEST_ASSERT_NOT_NULL(strstr(sent_payload, "\"MacAddress\":\"12:34:56:78:9A:BC\""));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_notification_send_true_status);
    RUN_TEST(test_notification_send_false_status);
    return UNITY_END();
}
