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
#include "leds.h"
#include "dht11.h"
#include "soil.h"
#include "pump.h"

#include "mqtt_topics.h"
#include "mqtt_received_publish.h"

extern volatile mqtt_received_publish_t mqtt_received_publish_array[10];
extern volatile int mqtt_received_publish_array_last;

void loop(){

    char *topic = "greenhouse/sensors";

    uint8_t humidity_int, humidity_dec, temperature_int, temperature_dec;
    uint16_t soil_humidity;

    dht11_get(&humidity_int, &humidity_dec, &temperature_int, &temperature_dec);

    soil_humidity = soil_read();

    uint16_t light_int = light_read();
    char payload[90] = "";
    sprintf(payload, "{\"air_temperature\":%d,\"air_humidity\":%d,\"soil_humidity\":%d,\"light_level\":%d}", temperature_int, humidity_int, soil_humidity, light_int);

    if( 50 > soil_humidity )   pump_on();
    else    pump_off();

    mqtt_publish( topic, payload, 0, 0, 0 );

}

int main()
{

    mqtt_init();
    light_init();
    dht11_init();
    soil_init();
    leds_init();
    light_init();
    pump_init();
    display_init();

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

    periodic_task_init_a( loop, 5000 );

    while(1){

        if(mqtt_received_publish_array_last > -1)
        {

            mqtt_received_publish_t temp = mqtt_received_publish_array[mqtt_received_publish_array_last--];

            char* temp_topic = mqtt_received_publish_get_topic( temp );
            char* temp_payload = mqtt_received_publish_get_payload( temp );

            char *topic = strstr( temp_topic, "/" );
            topic = strstr( ++topic, "/" );
            topic++;
            
            if (strcmp(topic, "light") == 0) {
                if (strcmp(temp_payload, "ON") == 0) leds_turnOn(2);
                else if (strcmp(temp_payload, "OFF") == 0) leds_turnOff(2);
                else uart_send_string_blocking(USART_0, "1\n");
            } else if (strcmp(topic, "watering") == 0) {
                if (strcmp(temp_payload, "ON") == 0) pump_off();
                else if (strcmp(temp_payload, "OFF") == 0) pump_on();
                else uart_send_string_blocking(USART_0, "2\n");
            } else {
                uart_send_string_blocking(USART_0, "3\n");
            }
        }
    }

    return 0;
}