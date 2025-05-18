#pragma once

#include <stdint.h>

typedef enum {
    USART_0,
    USART_1,
    USART_2,
    USART_3
} USART_t;

typedef void (*UART_Callback_t)(uint8_t);

void uart_send_string_blocking(USART_t port, char *str);
void uart_send_array_blocking(USART_t port, uint8_t *data, uint16_t len);
void uart_send_array_nonBlocking(USART_t port, uint8_t *data, uint16_t len);
void uart_init(USART_t port, uint32_t baud, UART_Callback_t callback);
UART_Callback_t uart_get_rx_callback(USART_t port);
