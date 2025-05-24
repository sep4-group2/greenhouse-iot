#include "unity.h"
#include "timestamp.h"
#include "wifi.h"
#include "uart.h"

#include "../fff.h"

FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_create_TCP_connection,
                char*, uint16_t, WIFI_TCP_Callback_t, char*, int*)
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_TCP_transmit,
                uint8_t*, uint16_t)
FAKE_VOID_FUNC(uart_send_string_blocking, USART_t, char*)

WIFI_ERROR_MESSAGE_t my_tcp_conn_mock(char* ip, uint16_t port, WIFI_TCP_Callback_t cb, char* buf, int* len) {
    cb(); // Dispara la simulación del callback que llamará http_response_callback()
    *len = 0;
    return WIFI_OK;
}

void setUp(void) {
    FFF_RESET_HISTORY();
    RESET_FAKE(wifi_command_create_TCP_connection)
    RESET_FAKE(wifi_command_TCP_transmit)
    RESET_FAKE(uart_send_string_blocking)

    wifi_command_create_TCP_connection_fake.custom_fake = my_tcp_conn_mock;
    wifi_command_TCP_transmit_fake.return_val = WIFI_OK;
}

void tearDown(void) {}

void test_timestamp_sync_success(void) {
    bool success = timestamp_sync_via_http();
    TEST_ASSERT_TRUE(success);

    uint8_t h, m, s, d, mo;
    uint16_t y;
    timestamp_get(&h, &m, &s);
    timestamp_get_date(&d, &mo, &y);

    TEST_ASSERT_EQUAL_UINT8(15, h);
    TEST_ASSERT_EQUAL_UINT8(34, m);
    TEST_ASSERT_EQUAL_UINT8(12, s);
    TEST_ASSERT_EQUAL_UINT8(22, d);
    TEST_ASSERT_EQUAL_UINT8(5, mo);
    TEST_ASSERT_EQUAL_UINT16(2024, y);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_timestamp_sync_success);
    return UNITY_END();
}
