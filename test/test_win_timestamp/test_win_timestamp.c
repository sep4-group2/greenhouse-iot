#include "unity.h"
#include "timestamp.h"
#include "fff.h"
#include <string.h>

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_create_TCP_connection,
               char*, uint16_t, WIFI_TCP_Callback_t, char*, int*);
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_TCP_transmit,
               uint8_t*, uint16_t);
FAKE_VOID_FUNC(uart_send_string_blocking, USART_t, char*);

extern WIFI_TCP_Callback_t fake_callback;

void setUp(void) {
    RESET_FAKE(wifi_command_create_TCP_connection);
    RESET_FAKE(wifi_command_TCP_transmit);
    RESET_FAKE(uart_send_string_blocking);
    FFF_RESET_HISTORY();
    timestamp_init();
}

void tearDown(void) {}

void test_timestamp_sync_via_http_should_parse_date_header_and_set_time(void) {
    char buffer[555] = {0};
    int len = 0;

    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Date: Sun, 11 May 2025 13:14:15 GMT\r\n"
        "Content-Type: application/json\r\n\r\n";

    strcpy(buffer, response);

    wifi_command_create_TCP_connection_fake.return_val = WIFI_OK;
    wifi_command_TCP_transmit_fake.return_val = WIFI_OK;

    TEST_ASSERT_TRUE(timestamp_sync_via_http());

    uint8_t h, m, s;
    uint8_t d, mo;
    uint16_t y;

    timestamp_get(&h, &m, &s);
    timestamp_get_date(&d, &mo, &y);

    TEST_ASSERT_EQUAL_UINT8(13, h);
    TEST_ASSERT_EQUAL_UINT8(14, m);
    TEST_ASSERT_EQUAL_UINT8(15, s);
    TEST_ASSERT_EQUAL_UINT8(12, d);
    TEST_ASSERT_EQUAL_UINT8(5, mo);  
    TEST_ASSERT_EQUAL_UINT16(2024, y);
}
