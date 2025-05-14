#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "timestamp.h"
#include "wifi.h"
#include "uart.h"

static uint8_t hours, minutes, seconds;

#define HTTP_BUFFER_SIZE 1500
static int resp_index = 0;
static char *resp_buf = NULL;

void timestamp_init(void) {
    hours = 0;
    minutes = 0;
    seconds = 0;
}

void timestamp_set(uint8_t h, uint8_t m, uint8_t s) {
    hours = h;
    minutes = m;
    seconds = s;
}

void http_response_callback(uint8_t byte) {
    if (resp_buf && resp_index < HTTP_BUFFER_SIZE - 1) {
        resp_buf[resp_index++] = byte;
        resp_buf[resp_index] = '\0';
    }
}

bool timestamp_sync_via_http(void) {
    char recv_buf[HTTP_BUFFER_SIZE] = {0};
    resp_index = 0;
    resp_buf = recv_buf;

    if (wifi_command_create_TCP_connection("example.com", 80, http_response_callback, recv_buf, NULL) != WIFI_OK) {
        uart_send_string_blocking(USART_0, "Error server\n");
        return false;
    }

    const char http_request[] =
        "GET / HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Connection: close\r\n"
        "Cache-Control: no-cache\r\n"
        "Pragma: no-cache\r\n\r\n";

    if (wifi_command_TCP_transmit((uint8_t *)http_request, strlen(http_request)) != WIFI_OK) {
        uart_send_string_blocking(USART_0, "Error HTTP\n");
        return false;
    }

    for (uint16_t waited = 0; waited < 3000; waited += 10) {
        _delay_ms(10);
        if (strstr(recv_buf, "\r\n\r\n")) break;
    }

    char *date_ptr = strstr(recv_buf, "Date: ");
    if (date_ptr) {
        char date_header[40] = {0};
        strncpy(date_header, date_ptr + 6, 29);
        uart_send_string_blocking(USART_0, "Timestamp: ");
        uart_send_string_blocking(USART_0, date_header);
        uart_send_string_blocking(USART_0, "\n");

        int h, m, s;
        if (sscanf(date_header + 17, "%2d:%2d:%2d", &h, &m, &s) == 3) {
            timestamp_set(h, m, s);
        }
        return true;
    }

    uart_send_string_blocking(USART_0, "date no found\n");
    return false;
}
