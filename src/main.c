#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "mqtt.h"
#include "periodic_task.h"
#include "light.h"
#include "dht11.h"

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

    dht11_get(&humidity_int, &humidity_dec, &temperature_int, &temperature_dec);

    uint16_t light_int = light_read();
    char payload[80] = "";
    sprintf(payload, "{\"air_temperature\":%d,\"air_humidity\":%d,\"soil_humidity\":%d,\"light_level\":%d}", temperature_int, humidity_int, 20, light_int);

    mqtt_publish( topic, payload, 0, 0, 0 );

}

int main()
{

    mqtt_init();
    light_init();
    dht11_init();

    sei();

    char *ssid = "ssid";
    char *password = "password";
    char *ip = "ip";
    int port = 1883;
    char *client_id = "client1";

    mqtt_connect( ssid, password, ip, port, client_id );

    periodic_task_init_a(loop, 5000);

    while(1){

    }

    return 0;
}