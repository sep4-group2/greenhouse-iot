#pragma once
#include <stdint.h>

void mqtt_init();

void mqtt_connect(char *ssid, char *password, char *ip, uint16_t port, char *client_id);

void mqtt_publish(char *topic, char *payload,
    int dup_flag, int qos_flag, int retained_flag);
