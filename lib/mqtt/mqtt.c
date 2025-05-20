#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "wifi.h"
#include "uart.h"
#include "mqtt_topics.h"
#include "mqtt_received_publish.h"

#include "MQTTPacket.h"

#define MQTT_RECEIVED_MESSAGE_BUF_SIZE 256

extern char mac_address[18];

volatile mqtt_received_publish_t mqtt_received_publish_array[25];
volatile int mqtt_received_publish_array_last = -1;
volatile int mqtt_received_publish_payload_len;

unsigned char mqtt_received_message_buf[MQTT_RECEIVED_MESSAGE_BUF_SIZE];
int mqtt_received_message_length;

static int create_connect_packet ( unsigned char *buf, int buflen, char *client_id );
static int create_publish_packet ( 
    char *topic, char *payload, char *buf, int buflen,
    int dup_flag, int qos_flag, int retained_flag, short packet_id );
static int create_subscribe_packet ( mqtt_topics_t topics, char *buf, int buflen, int dup_flag, short packet_id, int qos[] );
static void process_single_packet( unsigned char packet_type, char* buf, int len );
static void clear_received_message_buf();
static void get_topic_with_address( char *topic_dest, char *topic );
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

    char transmit_buf[1028];
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

WIFI_ERROR_MESSAGE_t mqtt_subscribe( mqtt_topics_t topics, int dup_flag, int qos_flags[] )
{
    short packet_id = 1;

    char transmit_buf[1028];
    int transmit_buflen = sizeof(transmit_buf);

    int transmit_len = create_subscribe_packet(
        topics, transmit_buf, transmit_buflen,
        dup_flag, packet_id, qos_flags 
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

static int create_subscribe_packet( mqtt_topics_t topics, char *buf, int buflen, int dup_flag, short packet_id, int qos[] )
{

    int count = mqtt_topics_get_topic_count( topics );
    char** topics_strings = mqtt_topics_get_topics( topics );
    MQTTString topics_to_subscribe[count];

    for (size_t i = 0; i < count; i++)
    {
        MQTTString topicString = MQTTString_initializer;
        topicString.cstring = topics_strings[i];

        topics_to_subscribe[i] = topicString;
    }

    int len = 0;

    len = MQTTSerialize_subscribe (
        buf, buflen, dup_flag, packet_id, count, topics_to_subscribe, qos
    );

    return len;
}

// method implementations for receiving data

static void clear_received_message_buf() 
{

    for (uint16_t i = 0; i < MQTT_RECEIVED_MESSAGE_BUF_SIZE; i++)
        mqtt_received_message_buf[i] = '\0';
    
}

static void get_topic_with_address( char *topic_dest, char *topic ){

    char temp_topic[50] = "greenhouse/";
    strcat(temp_topic, mac_address);
    strcat(temp_topic, "/");
    strcat(temp_topic, topic);
    strcpy( topic_dest, temp_topic);
    
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
    unsigned char* payload;


    switch ( packet_type ) 
    {

    case CONNACK:

        if ( MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, len) == 1 ) {

            char topic1[50];
            char topic2[50];
            char topic3[50];
            char topic4[60];

            get_topic_with_address( topic1, "light" );
            get_topic_with_address( topic2, "watering" );
            get_topic_with_address( topic3, "preset" );
            get_topic_with_address( topic4, "fertilizer" );

            char *subscribe_topics[] = { 
                topic1, 
                topic2, 
                topic3, 
                topic4 
            };

            mqtt_topics_t topics = mqtt_topics_init( subscribe_topics, 4);
            int qos[] = { 0, 0, 0, 0 };

            mqtt_subscribe( topics, 0, qos );

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
            &topic, &payload, &mqtt_received_publish_payload_len,
            buf, len
        ) == 1 )
        {

            char *topic_copy = malloc(topic.lenstring.len + 1);
            if (topic_copy != NULL) {
                memcpy(topic_copy, topic.lenstring.data, topic.lenstring.len);
                topic_copy[topic.lenstring.len] = '\0';
            }

            mqtt_received_publish_t received_message = mqtt_received_publish_init( payload, mqtt_received_publish_payload_len, topic_copy, topic.lenstring.len );
            mqtt_received_publish_array[++mqtt_received_publish_array_last] = received_message;

            if (qos == 1) {
                unsigned char puback_buf[50];
                int puback_buf_len = MQTTSerialize_puback(puback_buf, sizeof(puback_buf), packetId);
                wifi_command_TCP_transmit(puback_buf, puback_buf_len);
            }
            
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

    while (pos < total_bytes_received)
    {
        if ((pos + 1) >= total_bytes_received) break;

        unsigned char packet_type = mqtt_received_message_buf[pos] >> 4;

        int rem_len;
        int consumed_bytes = MQTTPacket_decodeBuf(&mqtt_received_message_buf[pos + 1], &rem_len);

        int packet_total_len = 1 + consumed_bytes + rem_len;

        if ((pos + packet_total_len) > total_bytes_received) break;
        process_single_packet(packet_type, &mqtt_received_message_buf[pos], packet_total_len);
        pos += packet_total_len;
    }

    clear_received_message_buf();

    return ;

}