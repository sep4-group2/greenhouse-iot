#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "timestamp.h"
#include "wifi.h"
#include "uart.h"



#define HTTP_BUFFER_SIZE 512

static uint8_t hours, minutes, seconds;
static uint8_t day, month, year;

static char *resp_buf = NULL;
static int resp_index = 0;

static const char *months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void timestamp_init(void) {
    hours = 0;
    minutes = 0;
    seconds = 0;
    day = 1;
    month = 1;
    year = 0;
}

void timestamp_set(uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint8_t y) {
    hours = h;
    minutes = m;
    seconds = s;
    day = d;
    month = mo;
    year = y;
}

void timestamp_get(uint8_t *h, uint8_t *m, uint8_t *s) {
    *h = hours;
    *m = minutes;
    *s = seconds;
}

void timestamp_get_date(uint8_t *d, uint8_t *mo, uint8_t *y) {
    *d = day;
    *mo = month;
    *y = year;
}

void http_response_callback(uint8_t byte) {
    if (resp_buf && resp_index < HTTP_BUFFER_SIZE - 1) {
        resp_buf[resp_index++] = byte;
        resp_buf[resp_index] = '\0';
    }
}

bool timestamp_sync_via_http(void) {
    static char recv_buf[HTTP_BUFFER_SIZE] = {0};
    int recv_len = 0;

    resp_buf = recv_buf;
    resp_index = 0;
    memset(recv_buf, 0, sizeof(recv_buf));

    uart_send_string_blocking(USART_0, "🌐 HTTP...\n");

    if (wifi_command_create_TCP_connection("neverssl.com", 80, http_response_callback, recv_buf, &recv_len) != WIFI_OK) {
        uart_send_string_blocking(USART_0, "Error TCP with site\n");
        return false;
    }

    const char http_request[] =
        "GET / HTTP/1.1\r\n"
        "Host: neverssl.com\r\n"
        "Connection: close\r\n"
        "Cache-Control: no-cache\r\n"
        "Pragma: no-cache\r\n\r\n";

    if (wifi_command_TCP_transmit((uint8_t *)http_request, strlen(http_request)) != WIFI_OK) {
        uart_send_string_blocking(USART_0, "Error request HTTP\n");
        return false;
    }

    uint16_t timeout_ms = 10000;
    uint16_t waited = 0;
    while (waited < timeout_ms) {
        _delay_ms(10);
        waited += 10;
        if (strstr(recv_buf, "\r\n\r\n")) {
            break;
        }
    }

    char *date_ptr = strstr(recv_buf, "Date: ");
    if (date_ptr) {
        char date_header[40] = {0};
        strncpy(date_header, date_ptr + 6, 29);  //

        int d, y, h, m, s;
        char month_str[4] = {0};

        if (sscanf(date_header, "%*[^,], %d %3s %d %2d:%2d:%2d",
                   &d, month_str, &y, &h, &m, &s) == 6) {

            int mo = 0;
            for (int i = 0; i < 12; i++) {
                if (strncmp(month_str, months[i], 3) == 0) {
                    mo = i + 1;
                    break;
                }
            }

            timestamp_set(h, m, s, d, mo, y % 100); 

            char buf[64];
            sprintf(buf, "📆 Timestamp: 20%02d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hours, minutes, seconds);
            uart_send_string_blocking(USART_0, buf);
            return true;
        } else {
            uart_send_string_blocking(USART_0, "Error parsing date\n");
        }
    } else {
        uart_send_string_blocking(USART_0, "Date no found\n");
    }

    return false;
}
