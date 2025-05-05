#include <stdint.h>
#include <string.h>

#include "wifi.h"
#include "uart.h"

#include "MQTTPacket.h"

void mqtt_init()
{

    uart_init(USART_0, 9600, NULL);
    uart_send_string_blocking(USART_0, "Initializing mqtt driver...\n");

    wifi_init();

}

void mqtt_connect( char *ssid, char *password, char *ip, uint16_t port, char *client_id )
{

    WIFI_ERROR_MESSAGE_t wifi_network_connect_message = wifi_command_join_AP( *ssid, *password );

    if (wifi_network_connect_message != WIFI_OK)
    {
        uart_send_string_blocking(USART_0, "Error connecting to wifi!\n");
        return;
    }
    
    char wifi_network_message[100];
    sprintf(wifi_network_message, "Connected to wifi at - ssid: %s | password: %s !", *ssid, *password);
    uart_send_string_blocking(USART_0, wifi_network_message);

    WIFI_ERROR_MESSAGE_t wifi_tcp_connect_message = wifi_command_create_TCP_connection( *ip, port, NULL, NULL);
    if (wifi_tcp_connect_message != WIFI_OK)
    {
        uart_send_string_blocking(USART_0, "Error making tcp connection!\n");
        return;
    }

    char wifi_tcp_message[100];
    sprintf(wifi_tcp_message, "Made tcp connection at - ip: %s | port: %d !", *ip, port);
    uart_send_string_blocking(USART_0, wifi_tcp_message);

    char connect_packet_buf[200];
    int connect_packet_buflen = sizeof(connect_packet_buf);
    int connect_packet_len = create_mqtt_connect_packet(connect_packet_buf, connect_packet_buflen);
    wifi_command_TCP_transmit(connect_packet_buf, connect_packet_len);
}

static int create_connect_packet(char *buf, int buflen, char *client_id)
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    int len = 0;

    data.clientID.cstring = *client_id;
    data.keepAliveInterval = 60;
    data.cleansession = 1;
    data.MQTTVersion = 3;

    len = MQTTSerialize_connect(buf, buflen, &data);
    return len;
}