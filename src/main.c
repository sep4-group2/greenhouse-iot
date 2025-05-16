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
#include "mqtt_topics.h"
#include "mqtt_received_publish.h"
#include "timestamp.h"
#include "clock.h"



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

    mqtt_publish( topic, payload, 0, 0, 0 );

}

int main()
{
    mqtt_init();
    light_init();
    dht11_init();
    soil_init();
    leds_init();
    display_init();
    timestamp_init();

    sei();

    char *ssid = "Xiaomi 12";
    char *password = "patty123";
    char *mqtt_ip = "192.168.150.208";
    int mqtt_port = 1883;
    char *client_id = "client1";

    if (wifi_command_set_mode_to_1() != WIFI_OK ||
        wifi_command_disable_echo() != WIFI_OK ||
        wifi_command_join_AP(ssid, password) != WIFI_OK) {
        uart_send_string_blocking(USART_0, "Error WiFi\n");
        while(1);
    }

    if (!timestamp_sync_via_http()) {
        uart_send_string_blocking(USART_0, "Error HTTP\n");
    }

    wifi_command_close_TCP_connection();

    if (mqtt_connect(ssid, password, mqtt_ip, mqtt_port, client_id) != WIFI_OK) {
        uart_send_string_blocking(USART_0, "Error MQTT\n");
        while(1);
    }

uint8_t hour = 0, minute = 0, second = 0;
uint8_t day_ = 0, month_ = 0;
uint16_t year_ = 0;

timestamp_get(&hour, &minute, &second);
timestamp_get_date(&day_, &month_, &year_);
clock_init(&global_clock, year_, month_, day_, hour, minute, second);


    periodic_task_init_c(clock_update_task, 1000);

    periodic_task_init_a(loop, 10000);

    while (1) {
        if (mqtt_received_publish_array_last > -1) {
            mqtt_received_publish_t temp = mqtt_received_publish_array[mqtt_received_publish_array_last--];
            char *temp_topic = mqtt_received_publish_get_topic(temp);
            char *temp_payload = mqtt_received_publish_get_payload(temp);

            char *topic = strstr(temp_topic, "/");
            topic = strstr(++topic, "/");
            topic++;

            if (strcmp(topic, "light") == 0) {
                if (strcmp(temp_payload, "ON") == 0) leds_turnOn(2);
                else if (strcmp(temp_payload, "OFF") == 0) leds_turnOff(2);
                else uart_send_string_blocking(USART_0, "1\n");
            } else if (strcmp(topic, "watering") == 0) {
                if (strcmp(temp_payload, "ON") == 0) leds_turnOn(3);
                else if (strcmp(temp_payload, "OFF") == 0) leds_turnOff(3);
                else uart_send_string_blocking(USART_0, "2\n");
            } else {
                uart_send_string_blocking(USART_0, "3\n");
            }
        }
    }

    return 0;
}
