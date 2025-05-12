#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>
// #include <time.h>

#include "mqtt.h"
#include "periodic_task.h"
#include "light.h"
#include "leds.h"
#include "dht11.h"
#include "soil.h"

extern volatile bool mqtt_publish_ready;
extern volatile unsigned char mqtt_received_publish_payload[512];
extern volatile unsigned char mqtt_received_publish_topic[512];
extern volatile int mqtt_received_publish_payload_len;

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

    mqtt_init();
    light_init();
    dht11_init();
    soil_init();
    leds_init();

    sei();

    char *ssid = "pixelphon";
    char *password = "poopdotcom";
    char *ip = "172.25.2.215";
    int port = 1883;
    char *client_id = "client1";

    mqtt_connect( ssid, password, ip, port, client_id );

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

        if(mqtt_publish_ready){

            char *topic = strstr( mqtt_received_publish_topic, "/" );
            topic = strstr( ++topic, "/" );
            topic++;

            if( strcmp( topic, "light" ) == 0 ){

                if( strcmp( mqtt_received_publish_payload, "ON" ) == 0 )
                {
                    
                    leds_turnOn(2);
                    mqtt_publish_ready = false;
                    uart_send_string_blocking(USART_0, "light: ON\n");

                }
                else if( strcmp( mqtt_received_publish_payload, "OFF" ) == 0 )
                {

                    leds_turnOff(2);
                    mqtt_publish_ready = false;
                    uart_send_string_blocking(USART_0, "light: OFF\n");

                }
                else 
                {

                    uart_send_string_blocking(USART_0, "got garbage publish\n");
                    mqtt_publish_ready = false;

                }

            }
            if( strcmp( mqtt_received_publish_topic, "watering" ) == 0 ){

                if( strcmp( mqtt_received_publish_payload, "ON" ) == 0 )
                {
                    
                    leds_turnOn(2);
                    mqtt_publish_ready = false;
                    uart_send_string_blocking(USART_0, "water: ON\n");

                }
                else if( strcmp( mqtt_received_publish_payload, "OFF" ) == 0 )
                {

                    leds_turnOff(2);
                    mqtt_publish_ready = false;
                    uart_send_string_blocking(USART_0, "water: OFF\n");

                }
                else 
                {

                    uart_send_string_blocking(USART_0, "got garbage publish\n");
                    mqtt_publish_ready = false;

                }
                
            }

            

        }
    }

    return 0;
}