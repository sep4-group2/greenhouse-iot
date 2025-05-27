#pragma once
#include <stdint.h>
#include "includes.h"
#include "wifi.h"
#include "mqtt_topics.h"

void mqtt_init();

WIFI_ERROR_MESSAGE_t mqtt_connect ( char *ssid, char *password, char *ip, uint16_t port, char *client_id );

WIFI_ERROR_MESSAGE_t mqtt_reconnect ( char *ip, uint16_t port, char *client_id );

WIFI_ERROR_MESSAGE_t mqtt_publish ( char *topic, char *payload,
    int dup_flag, int qos_flag, int retained_flag );

WIFI_ERROR_MESSAGE_t mqtt_subscribe ( mqtt_topics_t topics, int dup_flag, int qos_flags[] );

WIFI_ERROR_MESSAGE_t mqtt_pingreq( );
