#include "unity.h"
#include "mqtt.h"
#include "wifi.h"
#include "uart.h"
#include "mqtt_topics.h"

#include "../fff.h"


FAKE_VOID_FUNC(wifi_init)
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_join_AP, char*, char*)
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_create_TCP_connection, char*, uint16_t, WIFI_TCP_Callback_t, char*, int*)
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_TCP_transmit, uint8_t*, uint16_t)
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_close_TCP_connection)
FAKE_VOID_FUNC(wifi_enqueue_data_packet, uint8_t*, uint16_t) 

FAKE_VOID_FUNC(uart_init, USART_t, uint32_t, UART_Callback_t)
FAKE_VOID_FUNC(uart_send_string_blocking, USART_t, char*)

FAKE_VALUE_FUNC(int, mqtt_topics_get_topic_count, mqtt_topics_t)
FAKE_VALUE_FUNC(char**, mqtt_topics_get_topics, mqtt_topics_t)



void setUp(void) {
    FFF_RESET_HISTORY();
    RESET_FAKE(wifi_command_join_AP)
    RESET_FAKE(wifi_command_create_TCP_connection)
    RESET_FAKE(wifi_command_TCP_transmit)
    RESET_FAKE(wifi_command_close_TCP_connection)
    RESET_FAKE(uart_init)
    RESET_FAKE(uart_send_string_blocking)
    RESET_FAKE(mqtt_topics_get_topic_count)
    RESET_FAKE(mqtt_topics_get_topics)
}

void tearDown(void) {}


void test_mqtt_init() {
    mqtt_init();
    TEST_ASSERT_EQUAL(1, uart_init_fake.call_count);
    TEST_ASSERT_EQUAL(1, uart_send_string_blocking_fake.call_count);
}

void test_mqtt_connect_success() {
    wifi_command_join_AP_fake.return_val = WIFI_OK;
    wifi_command_create_TCP_connection_fake.return_val = WIFI_OK;
    wifi_command_TCP_transmit_fake.return_val = WIFI_OK;

    WIFI_ERROR_MESSAGE_t res = mqtt_connect("ssid", "pass", "192.168.1.1", 1883, "client1");
    TEST_ASSERT_EQUAL(WIFI_OK, res);
}

void test_mqtt_publish_success() {
    wifi_command_TCP_transmit_fake.return_val = WIFI_OK;

    WIFI_ERROR_MESSAGE_t res = mqtt_publish("topic", "hello", 0, 0, 0);
    TEST_ASSERT_EQUAL(WIFI_OK, res);
}

void test_mqtt_subscribe_success() {
    mqtt_topics_get_topic_count_fake.return_val = 1;
    static char* topic = "topic1";
    mqtt_topics_get_topics_fake.return_val = &topic;

    wifi_command_TCP_transmit_fake.return_val = WIFI_OK;

    int qos[1] = {0};
    mqtt_topics_t dummy_topics = 0;
    WIFI_ERROR_MESSAGE_t res = mqtt_subscribe(dummy_topics, 0, qos);
    TEST_ASSERT_EQUAL(WIFI_OK, res);
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_mqtt_init);
    RUN_TEST(test_mqtt_connect_success);
    RUN_TEST(test_mqtt_publish_success);
    RUN_TEST(test_mqtt_subscribe_success);
    return UNITY_END();
}