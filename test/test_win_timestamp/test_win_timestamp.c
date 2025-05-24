#include "unity.h"
#include "../fff.h"
#include "timestamp.h"


#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    WIFI_OK,
    WIFI_FAIL,
    WIFI_ERROR_NOT_RECEIVING,
    WIFI_ERROR_RECEIVED_ERROR,
    WIFI_ERROR_RECEIVING_GARBAGE
} WIFI_ERROR_MESSAGE_t;

typedef void (*WIFI_TCP_Callback_t)(void);

typedef enum {
    USART_0
} USART_t;

bool timestamp_sync_via_http(void);
void timestamp_get(uint8_t *h, uint8_t *m, uint8_t *s);
void timestamp_get_date(uint8_t *d, uint8_t *mo, uint16_t *y);

FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_create_TCP_connection,
                char*, uint16_t, WIFI_TCP_Callback_t, char*, int*)
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, wifi_command_TCP_transmit,
                uint8_t*, uint16_t)
FAKE_VOID_FUNC(uart_send_string_blocking, USART_t, char*)

void setUp(void) {
    FFF_RESET_HISTORY();
    RESET_FAKE(wifi_command_create_TCP_connection);
    RESET_FAKE(wifi_command_TCP_transmit);
    RESET_FAKE(uart_send_string_blocking);
}

void tearDown(void) {}

WIFI_ERROR_MESSAGE_t wifi_command_create_TCP_connection_custom(
    char *ip, uint16_t port, WIFI_TCP_Callback_t cb, char *buf, int *len)
{
    strcpy(buf,
        "HTTP/1.1 200 OK\r\n"
        "Date: Wed, 22 May 2024 15:34:12 GMT\r\n"
        "Content-Type: text/plain\r\n\r\n");
    *len = strlen(buf);

    cb(); 
    return WIFI_OK;
}

void test_timestamp_sync_success(void) {
    wifi_command_create_TCP_connection_fake.custom_fake = wifi_command_create_TCP_connection_custom;
    wifi_command_TCP_transmit_fake.return_val = WIFI_OK;

    bool result = timestamp_sync_via_http();
    TEST_ASSERT_TRUE(result);

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


WIFI_ERROR_MESSAGE_t wifi_command_create_TCP_connection_custom_fail(
    char *ip, uint16_t port, WIFI_TCP_Callback_t cb, char *buf, int *len)
{
    return WIFI_FAIL;
}

WIFI_ERROR_MESSAGE_t wifi_command_create_TCP_connection_custom_no_date(
    char *ip, uint16_t port, WIFI_TCP_Callback_t cb, char *buf, int *len)
{
    strcpy(buf,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n\r\n");
    *len = strlen(buf);
    cb();
    return WIFI_OK;
}

void test_timestamp_sync_tcp_fail(void) {
    wifi_command_create_TCP_connection_fake.custom_fake = wifi_command_create_TCP_connection_custom_fail;

    bool result = timestamp_sync_via_http();
    TEST_ASSERT_FALSE(result);
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_timestamp_sync_success);
    RUN_TEST(test_timestamp_sync_tcp_fail);
  
    return UNITY_END();
}
