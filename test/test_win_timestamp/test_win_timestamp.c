#include "unity.h"
#include "../fff.h"
#include "timestamp.h"
#include "wifi.h"
#include "uart.h"
#include <string.h>

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_create_TCP_connection, char*, uint16_t, WIFI_TCP_Callback_t, char*, int*);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_TCP_transmit, uint8_t*, uint16_t);
FAKE_VOID_FUNC(uart_send_string_blocking, USART_t, char*);
FAKE_VOID_FUNC(_delay_ms, int);

// Fake de delay acumulativo
void fake_delay_ms(int ms) {
    static int total = 0;
    total += ms;
    if (total > 50) total = 0;
}

// Fake para simular respuesta HTTP
WIFI_ERROR_MESSAGE_t fake_tcp_conn(char* ip, uint16_t port, WIFI_TCP_Callback_t callback, char* buf, int* len) {
    const char* fake_response =
        "HTTP/1.1 200 OK\r\n"
        "Date: Wed, 15 May 2025 14:22:33 GMT\r\n"
        "Content-Type: application/json\r\n"
        "\r\n";
    for (size_t i = 0; i < strlen(fake_response); i++) {
        callback(fake_response[i]);
    }
    return WIFI_OK;
}

void setUp(void) {
    RESET_FAKE(wifi_command_create_TCP_connection);
    RESET_FAKE(wifi_command_TCP_transmit);
    RESET_FAKE(uart_send_string_blocking);
    RESET_FAKE(_delay_ms);
}

void tearDown(void) {}

void test_timestamp_set_and_get(void) {
    timestamp_set(12, 34, 56, 7, 8, 2025);

    uint8_t h, m, s, d, mo;
    uint16_t y;

    timestamp_get(&h, &m, &s);
    timestamp_get_date(&d, &mo, &y);

    TEST_ASSERT_EQUAL_UINT8(12, h);
    TEST_ASSERT_EQUAL_UINT8(34, m);
    TEST_ASSERT_EQUAL_UINT8(56, s);
    TEST_ASSERT_EQUAL_UINT8(7, d);
    TEST_ASSERT_EQUAL_UINT8(8, mo);
    TEST_ASSERT_EQUAL_UINT16(2025, y);
}

void test_timestamp_sync_via_http_success(void) {
    wifi_command_create_TCP_connection_fake.custom_fake = fake_tcp_conn;
    wifi_command_TCP_transmit_fake.return_val = WIFI_OK;
    _delay_ms_fake.custom_fake = fake_delay_ms;

    TEST_ASSERT_TRUE(timestamp_sync_via_http());
    TEST_ASSERT_EQUAL(1, uart_send_string_blocking_fake.call_count);
}

void test_timestamp_sync_via_http_tcp_fail(void) {
    wifi_command_create_TCP_connection_fake.return_val = WIFI_FAIL;

    TEST_ASSERT_FALSE(timestamp_sync_via_http());
    TEST_ASSERT_EQUAL_STRING("Error TCP with site\n", uart_send_string_blocking_fake.arg1_val);
}

void test_timestamp_sync_via_http_transmit_fail(void) {
    wifi_command_create_TCP_connection_fake.return_val = WIFI_OK;
    wifi_command_TCP_transmit_fake.return_val = WIFI_FAIL;

    TEST_ASSERT_FALSE(timestamp_sync_via_http());
    TEST_ASSERT_EQUAL_STRING("Error request HTTP\n", uart_send_string_blocking_fake.arg1_val);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_timestamp_set_and_get);
    RUN_TEST(test_timestamp_sync_via_http_success);
    RUN_TEST(test_timestamp_sync_via_http_tcp_fail);
    RUN_TEST(test_timestamp_sync_via_http_transmit_fail);
    return UNITY_END();
}
