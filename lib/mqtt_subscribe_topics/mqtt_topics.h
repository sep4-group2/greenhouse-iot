#pragma once

typedef struct mqtt_topics* mqtt_topics_t;

mqtt_topics_t mqtt_topics_init( char *topics[], int count );
int mqtt_topics_get_topic_count( mqtt_topics_t self );
char** mqtt_topics_get_topics( mqtt_topics_t self );
