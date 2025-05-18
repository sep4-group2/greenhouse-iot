#pragma once

#include <stdint.h>

typedef enum {
    WIFI_OK,
    WIFI_FAIL,
    WIFI_ERROR_NOT_RECEIVING,
    WIFI_ERROR_RECEIVED_ERROR,
    WIFI_ERROR_RECEIVING_GARBAGE
} WIFI_ERROR_MESSAGE_t;

typedef void (*WIFI_TCP_Callback_t)(uint8_t);

WIFI_ERROR_MESSAGE_t wifi_command_create_TCP_connection(
    char *IP,
    uint16_t port,
    WIFI_TCP_Callback_t callback_when_message_received,
    char *received_message_buffer,
    int *received_message_length
);

WIFI_ERROR_MESSAGE_t wifi_command_TCP_transmit(uint8_t *data, uint16_t length);
