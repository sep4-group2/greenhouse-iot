#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "light.h"
#include "display.h"
#include "mqtt.h"
#include "periodic_task.h"
#include "light.h"
#include "dht11.h"
#include "soil.h"

// static uint8_t _buff[100];
// static uint8_t _index = 0;
// volatile static bool _done = false;
// void console_rx(uint8_t _rx)
// {
//     uart_send_blocking(USART_0, _rx);
//     if(('\r' != _rx) && ('\n' != _rx))
//     {
//         if(_index < 100-1)
//         {
//             _buff[_index++] = _rx;
//         }
//     }
//     else
//     {
//         _buff[_index] = '\0';
//         _index = 0;
//         _done = true;
//         uart_send_blocking(USART_0, '\n');
// //        uart_send_string_blocking(USART_0, (char*)_buff);
//     }
// }

void loop(){

    char *topic = "greenhouse/sensors";

    uint8_t humidity_int, humidity_dec, temperature_int, temperature_dec;
    uint16_t soil_humidity;

    dht11_get(&humidity_int, &humidity_dec, &temperature_int, &temperature_dec);

    soil_humidity = soil_read();

    uint16_t light_int = light_read();
    char payload[90] = "";
    sprintf(payload, "{\"air_temperature\":%d,\"air_humidity\":%d,\"soil_humidity\":%d,\"light_level\":%d}", temperature_int, humidity_int, soil_humidity, light_int);

    mqtt_publish( topic, payload, 0, 0, 0 );

}

int main()
{
    char welcome_text[] = "Welcome from SEP4 IoT hardware!\n";
    char prompt_text[] = "Type text to send: ";

    mqtt_init();
    light_init();
    dht11_init();
    soil_init();
    light_init();
    display_init();

    sei();

    char *ssid = "pixelphon";
    char *password = "poopdotcom";
    char *ip = "172.25.2.215";
    int port = 1883;
    char *client_id = "client1";

    WIFI_ERROR_MESSAGE_t mqtt_result = mqtt_connect( ssid, password, ip, port, client_id );

    // if(mqtt_result != WIFI_OK)
    // {
    //     for (int i = 0; i < 3; ++i) {
    //         if (mqtt_reconnect( ip, port, client_id ) == WIFI_OK)
    //             break;
    //         _delay_ms(1000); // Delay before retry
    //     }
    // }

    // _delay_ms(500); 

    periodic_task_init_a( loop, 10000 );

    while(1){

        // char time_message[70];

        // time_t t = time(NULL);
        // struct tm tm = *localtime(&t);
        // sprintf(time_message, "now: %s-%02s-%02s %02s:%02s:%02s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        // uart_send_string_blocking(USART_0, time_message);

        // _delay_ms(200);
    }

    return 0;
}