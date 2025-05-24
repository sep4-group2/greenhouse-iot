#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "uart_packet.h"

#define BUFFER_CAPACITY 25

typedef struct uart_buffer* uart_buffer_t;

uart_buffer_t uart_buffer_init();

bool uart_buffer_push ( uart_buffer_t self, uart_packet_t item );

uart_packet_t uart_buffer_pop( uart_buffer_t self );

int uart_buffer_get_count( uart_buffer_t self );

bool uart_buffer_is_empty( uart_buffer_t self );

bool uart_buffer_is_full( uart_buffer_t self );