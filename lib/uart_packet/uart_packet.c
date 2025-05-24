#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uart_packet.h"

typedef struct uart_packet {
    uint8_t *str;
    uint16_t len;
} uart_packet;

uart_packet_t uart_packet_init( uint8_t *str, uint16_t len ){

    uart_packet_t _new_packet = calloc( 1, sizeof(uart_packet) );

    _new_packet->str = malloc(len + 1);
    if (NULL == _new_packet->str || NULL == str || NULL == _new_packet ) {
        free(_new_packet);
        return NULL;
    }

    memcpy( _new_packet->str, str, len );
    _new_packet-> len = len;

    return _new_packet;

}

uint8_t *uart_packet_get_str( uart_packet_t self ){
    return self->str;
}

int uart_packet_get_len( uart_packet_t self ){
    return self->len;
}

void uart_packet_free(uart_packet_t self) {
    if (self) {
        if (self->str) {
            free(self->str);
        }
        free(self);
    }
}