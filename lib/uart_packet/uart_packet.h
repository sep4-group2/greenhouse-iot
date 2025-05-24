#include <stdint.h>

typedef struct uart_packet *uart_packet_t;

uart_packet_t uart_packet_init( uint8_t *str, uint16_t len );

uint8_t *uart_packet_get_str( uart_packet_t self );

int uart_packet_get_len( uart_packet_t self );

void uart_packet_free( uart_packet_t self );