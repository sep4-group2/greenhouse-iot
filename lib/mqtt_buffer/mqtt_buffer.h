#pragma once

#include <stdlib.h>

#include "mqtt_received_packet.h"

#define BUFFER_CAPACITY 25

typedef struct mqtt_buffer* mqtt_buffer_t;

mqtt_buffer_t mqtt_buffer_init();

bool mqtt_buffer_push( mqtt_buffer_t self, mqtt_received_packet_t item );

mqtt_received_packet_t mqtt_buffer_pop( mqtt_buffer_t self );

bool mqtt_buffer_is_empty( mqtt_buffer_t self );