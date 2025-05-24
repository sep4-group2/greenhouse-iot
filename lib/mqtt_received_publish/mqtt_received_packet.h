#pragma once

typedef struct mqtt_received_packet *mqtt_received_packet_t;

mqtt_received_packet_t mqtt_received_packet_init( unsigned char packet_type, char* buf, int len );

char *mqtt_received_packet_get_buf( mqtt_received_packet_t self );

int mqtt_received_packet_get_len( mqtt_received_packet_t self );

char mqtt_received_packet_get_type( mqtt_received_packet_t self );

void mqtt_received_packet_free( mqtt_received_packet_t self );