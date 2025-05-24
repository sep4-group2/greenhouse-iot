#include <stdlib.h>
#include <string.h>

#include "mqtt_received_packet.h"

typedef struct mqtt_received_packet {
    unsigned char packet_type; 
    char* buf;
    int len;
} mqtt_received_packet;

mqtt_received_packet_t mqtt_received_packet_init( unsigned char packet_type, char* buf, int len ){

    mqtt_received_packet_t _received_packet = calloc( 1, sizeof(mqtt_received_packet) );

    _received_packet->buf = malloc(len + 1);
    if (NULL == _received_packet->buf || NULL == buf || NULL == _received_packet ) {
        free(_received_packet);
        return NULL;
    }

    memcpy( _received_packet->buf, buf, len );

    _received_packet->packet_type = packet_type;
    _received_packet-> len = len;

    return _received_packet;

}

char *mqtt_received_packet_get_buf( mqtt_received_packet_t self ){
    return self->buf;
}

int mqtt_received_packet_get_len( mqtt_received_packet_t self ){
    return self->len;
}

char mqtt_received_packet_get_type( mqtt_received_packet_t self ){
    return self->packet_type;
}

void mqtt_received_packet_free( mqtt_received_packet_t self ){
    if(NULL!= self) free(self);
}