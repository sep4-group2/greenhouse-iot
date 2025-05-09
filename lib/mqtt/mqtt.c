#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "wifi.h"
#include "uart.h"

#include "MQTTPacket.h"

#define MQTT_RECEIVED_MESSAGE_BUF_SIZE 256
volatile bool mqtt_publish_ready = false;
volatile char mqtt_publish_buffer[128];

unsigned char mqtt_received_message_buf[MQTT_RECEIVED_MESSAGE_BUF_SIZE];
int mqtt_received_message_length;

static int create_connect_packet ( unsigned char *buf, int buflen, char *client_id );
static int create_publish_packet ( 
    char *topic, char *payload, char *buf, int buflen,
    int dup_flag, int qos_flag, int retained_flag, short packet_id );
static int create_subscribe_packet ( char *topic, char *buf, int buflen, int dup_flag, short packet_id, int count, int qos );
static void process_single_packet( unsigned char packet_type, char* buf, int len );
static void clear_received_message_buf();
WIFI_TCP_Callback_t callback_when_message_received();

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

    WIFI_ERROR_MESSAGE_t wifi_tcp_connect_message = wifi_command_create_TCP_connection(
         ip, port, callback_when_message_received, mqtt_received_message_buf, &mqtt_received_message_length
    );
    if (wifi_tcp_connect_message != WIFI_OK)
    {
        char wifi_tcp_error_message[100];
        sprintf(wifi_tcp_error_message, "Error making tcp connection! Error: %d\n", wifi_tcp_connect_message);
        uart_send_string_blocking(USART_0, wifi_tcp_error_message);
        return wifi_tcp_connect_message;
    }

    char wifi_tcp_message[100];
    sprintf( wifi_tcp_message, "Made tcp connection at - ip: %s | port: %d\n", ip, port );
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

    WIFI_ERROR_MESSAGE_t wifi_tcp_connect_message = wifi_command_create_TCP_connection(
        ip, port, callback_when_message_received, mqtt_received_message_buf, &mqtt_received_message_length
    );

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
        uart_send_string_blocking(USART_0, "Failed to serialize mqtt publish packet.\n");
        return WIFI_ERROR_RECEIVING_GARBAGE;
    }

    return wifi_command_TCP_transmit((uint8_t *)transmit_buf, transmit_len);
}

WIFI_ERROR_MESSAGE_t mqtt_subscribe( char *topic, int dup_flag, int qos_flag )
{
    short packet_id = 1;

    char transmit_buf[300];
    int transmit_buflen = sizeof(transmit_buf);

    int transmit_len = create_subscribe_packet(
        topic, transmit_buf, transmit_buflen,
        dup_flag, packet_id, 1, qos_flag 
    );

    if (transmit_len <= 0) {
        uart_send_string_blocking(USART_0, "Failed to serialize mqtt subscribe packet.\n");
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

static int create_subscribe_packet( char *topic, char *buf, int buflen, int dup_flag, short packet_id, int count, int qos )
{
    MQTTString topicString = MQTTString_initializer;

    topicString.cstring = topic;

    MQTTString topicStrings[] = { topicString };

    int requestedQoSs[] = { qos };

    int len = 0;

    len = MQTTSerialize_subscribe (
        buf, buflen, dup_flag, packet_id, count, topicStrings, requestedQoSs
    );

    return len;
}

// method implementations for receiving data

static void clear_received_message_buf() 
{

    for (uint16_t i = 0; i < MQTT_RECEIVED_MESSAGE_BUF_SIZE; i++)
        mqtt_received_message_buf[i] = '\0';
    
}

static void process_single_packet( unsigned char packet_type, char* buf, int len ) {

    // connack flags
    unsigned char sessionPresent;
    unsigned char connack_rc;

    // publish flags
    unsigned char dup;
    int qos;
    unsigned char retained;
    unsigned short packetId;
    MQTTString topic;
    unsigned char *payload;
    int payloadLen;

    switch ( packet_type ) 
    {

    case CONNACK:

        if ( MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, len) == 1 ) {

            char subscribe_topic[] = "greenhouse/control/light";
            mqtt_subscribe( subscribe_topic, 0, 0 );

        } else {

        }

    break;

    case SUBACK:

    break;

    case PUBACK:

    break;

    case PINGRESP:

    break;

    case PUBLISH:

        if( MQTTDeserialize_publish( 
            &dup, &qos, &retained, &packetId, 
            &topic, &payload, &payloadLen,
            buf, len
        ) == 1 )
        {

            strcpy( mqtt_publish_buffer, payload );
            mqtt_publish_ready = true;
            
        }
        else {

        }

    break;
    
    default:

    break;

    }
}

WIFI_TCP_Callback_t callback_when_message_received() 
{

    int pos = 0;
    int total_bytes_received = mqtt_received_message_length;

    while (pos < total_bytes_received) {

        unsigned char packet_type = mqtt_received_message_buf[pos] >> 4;

        int rem_len;
        int consumed_bytes = MQTTPacket_decodeBuf( &mqtt_received_message_buf[pos + 1], &rem_len );

        int packet_total_len = 1 + consumed_bytes + rem_len;

        process_single_packet( packet_type, &mqtt_received_message_buf[pos], packet_total_len);

        pos += packet_total_len;

    }

    clear_received_message_buf();

    return ;

}