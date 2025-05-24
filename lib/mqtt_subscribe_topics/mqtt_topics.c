#include <stdlib.h>
#include <string.h>

#include "mqtt_topics.h"

typedef struct mqtt_topics
{

	char **topics;
	int topic_count;

} mqtt_topics;

mqtt_topics_t mqtt_topics_init( char *topics[], int count ){
    
    mqtt_topics_t _subscribe_topics = calloc( 1, sizeof(mqtt_topics) );

    if( NULL == _subscribe_topics ) 
    { 
        return NULL;
    }

    _subscribe_topics->topic_count = count;

    _subscribe_topics->topics = malloc(_subscribe_topics->topic_count * sizeof(char *));
    if (_subscribe_topics->topics == NULL) 
    {
        free(_subscribe_topics);
        return NULL;
    }

    for (size_t i = 0; i < count; i++) 
    {

        _subscribe_topics->topics[i] = malloc(strlen(topics[i]) + 1);
        
        if (_subscribe_topics->topics[i] == NULL) {
            
            for (size_t j = 0; j < i; j++) {
                free(_subscribe_topics->topics[j]);
            }
            free(_subscribe_topics->topics);
            free(_subscribe_topics);
            return NULL;
        }
        strcpy(_subscribe_topics->topics[i], topics[i]);
    }

    return _subscribe_topics;
}

int mqtt_topics_get_topic_count( mqtt_topics_t self ){

    return self->topic_count;

}

char** mqtt_topics_get_topics( mqtt_topics_t self ){

    return self->topics;

}
