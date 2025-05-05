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
    char wifi_network_message[50];
    sprintf(wifi_network_message, "WIFI network connection got message: %d", wifi_network_connect_message);
    uart_send_string_blocking(USART_0, wifi_network_message);

    WIFI_ERROR_MESSAGE_t wifi_tcp_connect_message = wifi_command_create_TCP_connection( *ip, port, NULL, NULL);
    char wifi_tcp_message[50];
    sprintf(wifi_tcp_message, "TCP connection got message: %d", wifi_tcp_connect_message);
    uart_send_string_blocking(USART_0, wifi_tcp_message);

}