#pragma once

typedef struct mqtt_received_publish *mqtt_received_publish_t;

mqtt_received_publish_t mqtt_received_publish_init( char *payload, int payload_len, char *topic, int topic_len );
char *mqtt_received_publish_get_topic( mqtt_received_publish_t self );
char *mqtt_received_publish_get_payload( mqtt_received_publish_t self );
void mqtt_received_publish_free( mqtt_received_publish_t self );
