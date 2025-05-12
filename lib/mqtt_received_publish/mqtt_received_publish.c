#include <stdlib.h>
#include <string.h>

#include "mqtt_received_publish.h"

typedef struct mqtt_received_publish {
    char topic[128];
    char payload[128];
} mqtt_received_publish;

mqtt_received_publish_t mqtt_received_publish_init( char *payload, int payload_len, char *topic, int topic_len ){

    mqtt_received_publish_t _received_publish = calloc( 1, sizeof(mqtt_received_publish) );

    if( NULL == _received_publish ) 
    { 
        return NULL;
    }

    if(topic_len > sizeof(_received_publish->topic)) return NULL;
    if(payload_len > sizeof(_received_publish->payload)) return NULL;

    strncpy( _received_publish->payload, payload, payload_len );
    strncpy( _received_publish->topic, topic, topic_len );

    return _received_publish;

}

char *mqtt_received_publish_get_topic( mqtt_received_publish_t self ){
    return self->topic;
}

char *mqtt_received_publish_get_payload( mqtt_received_publish_t self ){
    return self->payload;
}

void mqtt_received_publish_free( mqtt_received_publish_t self ){
    if(NULL!= self) free(self);
}