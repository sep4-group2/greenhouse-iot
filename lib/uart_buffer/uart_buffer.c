#include <stdbool.h>
#include <stdlib.h>

#include "uart_buffer.h"
#include "uart_packet.h"

typedef struct uart_buffer {
    uart_packet_t buffer[BUFFER_CAPACITY];
    int head;
    int tail;
    int count;
} uart_buffer;

uart_buffer_t uart_buffer_init( ) {
    uart_buffer_t _new_buf = calloc( 1, sizeof(uart_buffer) );
    if( NULL == _new_buf ) return NULL;

    _new_buf->head = 0;
    _new_buf->tail = 0;
    _new_buf->count = 0;

    return _new_buf;
}

bool uart_buffer_push( uart_buffer_t self, uart_packet_t item ) {
    if ( NULL == self || self->count == BUFFER_CAPACITY ) return false;

    self->buffer[self->tail] = item;
    self->tail = (self->tail + 1) % BUFFER_CAPACITY;
    self->count++;
    return true;
}

uart_packet_t uart_buffer_pop( uart_buffer_t self ) {
    if (!self || self->count == 0) return NULL;

    uart_packet_t pkt = self->buffer[self->head];
    self->head = (self->head + 1) % BUFFER_CAPACITY;
    self->count--;
    return pkt;
}

int uart_buffer_get_count( uart_buffer_t self ) {
    if ( NULL == self ) return false;
    return self->count;
}

bool uart_buffer_is_empty( uart_buffer_t self ) {
    if ( NULL == self ) return false;
    return 0 == self->count;
}

bool uart_buffer_is_full( uart_buffer_t self ) {
    if ( NULL == self ) return false;
    return BUFFER_CAPACITY == self->count;
}