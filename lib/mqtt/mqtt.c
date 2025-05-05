#include <stdint.h>
#include <string.h>

#include "wifi.h"
#include "uart.h"

#include "MQTTPacket.h"

static int create_connect_packet( unsigned char *buf, int buflen, char *client_id );
static int create_publish_packet ( 
    char *topic, char *payload, char *buf, int buflen,
    int dup_flag, int qos_flag, int retained_flag, short packet_id );

// code implementations

void mqtt_init()
{

    uart_init(USART_0, 9600, NULL);
    uart_send_string_blocking(USART_0, "Initializing mqtt driver...\n");

    wifi_init();

}

WIFI_ERROR_MESSAGE_t mqtt_connect( char *ssid, char *password, char *ip, uint16_t port, char *client_id )
{

    WIFI_ERROR_MESSAGE_t wifi_network_connect_message = wifi_command_join_AP( ssid, password );

    if (wifi_network_connect_message != WIFI_OK)
    {
        uart_send_string_blocking(USART_0, "Error connecting to wifi!\n");
        return wifi_network_connect_message;
    }
    
    char wifi_network_message[100];
    sprintf(wifi_network_message, "Connected to wifi at - ssid: %s | password: %s\n", ssid, password);
    uart_send_string_blocking(USART_0, wifi_network_message);

    wifi_command_close_TCP_connection();

    WIFI_ERROR_MESSAGE_t wifi_tcp_connect_message = wifi_command_create_TCP_connection( ip, port, NULL, NULL);
    if (wifi_tcp_connect_message != WIFI_OK)
    {
        char wifi_tcp_error_message[100];
        sprintf(wifi_tcp_error_message, "Error making tcp connection! Error: %d\n", wifi_tcp_connect_message);
        uart_send_string_blocking(USART_0, wifi_tcp_error_message);
        return wifi_tcp_connect_message;
    }

    char wifi_tcp_message[100];
    sprintf(wifi_tcp_message, "Made tcp connection at - ip: %s | port: %d\n", ip, port);
    uart_send_string_blocking(USART_0, wifi_tcp_message);

    unsigned char connect_packet_buf[200];
    int connect_packet_buflen = sizeof(connect_packet_buf);
    int connect_packet_len = create_connect_packet(
        connect_packet_buf, connect_packet_buflen, client_id
    );

    WIFI_ERROR_MESSAGE_t wifi_mqtt_connect_message = wifi_command_TCP_transmit(connect_packet_buf, connect_packet_len);
    return wifi_mqtt_connect_message;

}

WIFI_ERROR_MESSAGE_t mqtt_reconnect( char *ip, uint16_t port, char *client_id )
{

    wifi_command_close_TCP_connection();

    WIFI_ERROR_MESSAGE_t wifi_tcp_connect_message = wifi_command_create_TCP_connection( ip, port, NULL, NULL);
    if (wifi_tcp_connect_message != WIFI_OK)
    {
        char wifi_tcp_error_message[100];
        sprintf(wifi_tcp_error_message, "Error making tcp connection! Error: %d\n", wifi_tcp_connect_message);
        uart_send_string_blocking(USART_0, wifi_tcp_error_message);
        return wifi_tcp_connect_message;
    }

    char wifi_tcp_message[100];
    sprintf(wifi_tcp_message, "Made tcp connection at - ip: %s | port: %d\n", ip, port);
    uart_send_string_blocking(USART_0, wifi_tcp_message);

    unsigned char connect_packet_buf[200];
    int connect_packet_buflen = sizeof(connect_packet_buf);
    int connect_packet_len = create_connect_packet(
        connect_packet_buf, connect_packet_buflen, client_id
    );

    WIFI_ERROR_MESSAGE_t wifi_mqtt_connect_message = wifi_command_TCP_transmit(connect_packet_buf, connect_packet_len);
    return wifi_mqtt_connect_message;

}

WIFI_ERROR_MESSAGE_t mqtt_publish( char *topic, char *payload, int dup_flag, int qos_flag, int retained_flag )
{
    short packet_id;

    if (qos_flag > 0)
        packet_id = 1;
    else
        packet_id = 0;

    char transmit_buf[300];
    int transmit_buflen = sizeof(transmit_buf);

    int transmit_len = create_publish_packet(
        topic, payload, transmit_buf, transmit_buflen,
        dup_flag, qos_flag, retained_flag, packet_id
    );

    if (transmit_len <= 0) {
        uart_send_string_blocking(USART_0, "Failed to serialize MQTT PUBLISH packet.\n");
        return WIFI_ERROR_RECEIVING_GARBAGE;
    }

    return wifi_command_TCP_transmit((uint8_t *)transmit_buf, transmit_len);
}


// helper functions for packet creation n stuff

static int create_connect_packet( unsigned char *buf, int buflen, char *client_id )
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    int len = 0;

    data.clientID.cstring = client_id;
    data.keepAliveInterval = 60;
    data.cleansession = 1;
    data.MQTTVersion = 3;

    len = MQTTSerialize_connect(buf, buflen, &data);
    return len;
}

static int create_publish_packet ( 
    char *topic, char *payload, char *buf, int buflen,
    int dup_flag, int qos_flag, int retained_flag, short packet_id )
{

    MQTTString topicString = MQTTString_initializer;
    int payloadlen = strlen(payload);
    int len = 0;

    topicString.cstring = topic;

    len = MQTTSerialize_publish(
        buf, buflen, dup_flag, qos_flag, retained_flag, 
        packet_id, topicString, payload, payloadlen
    );

    return len;

}