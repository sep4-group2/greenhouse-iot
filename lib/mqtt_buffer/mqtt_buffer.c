#include <stdbool.h>
#include <stdlib.h>

#include "mqtt_buffer.h"
#include "mqtt_received_packet.h"

typedef struct mqtt_buffer {
    mqtt_received_packet_t buffer[BUFFER_CAPACITY];
    int head;
    int tail;
    int count;
} mqtt_buffer;

mqtt_buffer_t mqtt_buffer_init( ) {
    mqtt_buffer_t _new_buf = calloc( 1, sizeof(mqtt_buffer) );
    if( NULL == _new_buf ) return NULL;

    _new_buf->head = 0;
    _new_buf->tail = 0;
    _new_buf->count = 0;

    return _new_buf;
}

bool mqtt_buffer_push( mqtt_buffer_t self, mqtt_received_packet_t item ) {
    if ( NULL == self || self->count == BUFFER_CAPACITY ) return false;

    self->buffer[self->tail] = item;
    self->tail = (self->tail + 1) % BUFFER_CAPACITY;
    self->count++;
    return true;
}

mqtt_received_packet_t mqtt_buffer_pop( mqtt_buffer_t self ) {
    if (!self || self->count == 0) return NULL;

    mqtt_received_packet_t pkt = self->buffer[self->head];
    self->head = (self->head + 1) % BUFFER_CAPACITY;
    self->count--;
    return pkt;
}

bool mqtt_buffer_is_empty( mqtt_buffer_t self ) {
    if ( NULL == self ) return false;
    return 0 == self->count;
}
