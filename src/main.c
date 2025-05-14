#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "wifi.h"
#include "light.h"
#include "display.h"
#include "mqtt.h"
#include "periodic_task.h"
#include "light.h"
#include "leds.h"
#include "dht11.h"
#include "soil.h"
#include "pump.h"
#include "preset.h"

#include "mqtt_topics.h"
#include "mqtt_received_publish.h"

extern volatile mqtt_received_publish_t mqtt_received_publish_array[25];
extern volatile int mqtt_received_publish_array_last;

static preset_t active_preset;
char mac_address[18];

char *extract_substring( char *start, char *end );

char *extract_from_json( char *to_extract, char *json );

void loop1();

void loop2();

int main()
{
    
    active_preset = preset_init();
    uart_init( USART_0, 9600, NULL );
    wifi_init();

    _delay_ms(4000);

    wifi_command_get_MAC(mac_address);

    _delay_ms(1000);

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

    mqtt_connect( ssid, password, ip, port, mac_address );

    // if(mqtt_result != WIFI_OK)
    // {
    //     for (int i = 0; i < 3; ++i) {
    //         if (mqtt_reconnect( ip, port, client_id ) == WIFI_OK)
    //             break;
    //         _delay_ms(1000); // Delay before retry
    //     }
    // }

    // _delay_ms(500); 

    periodic_task_init_a( loop1, 10000 );

    while(1){

        if(mqtt_received_publish_array_last > -1)
        {

            mqtt_received_publish_t temp = mqtt_received_publish_array[mqtt_received_publish_array_last--];

            char* temp_topic = mqtt_received_publish_get_topic( temp );
            char* temp_payload = mqtt_received_publish_get_payload( temp );


            while( NULL != strstr( temp_topic, "/" ) ){
                temp_topic = strstr( temp_topic, "/" );
                temp_topic++;
            }
            
            if (strcmp(temp_topic, "light") == 0) {

                leds_toggle(2);

            } else if (strcmp(temp_topic, "watering") == 0) {

                leds_toggle(3);

            } else if (strcmp(temp_topic, "preset") == 0) {

                char *watering_method = extract_from_json( "WateringMethod", temp_payload );

                if( 0 == strcmp( watering_method, "\"manual\"" ) ) preset_set_watering_method( active_preset, ACTION_MANUAL );
                else if( 0 == strcmp( watering_method, "\"automated\"" ) ) preset_set_watering_method( active_preset, ACTION_AUTOMATED );

                char *min_soil_humidity_string = extract_from_json( "MinSoilHumidity", temp_payload );
                int min_soil_humidity = atoi( min_soil_humidity_string );
                preset_set_min_soil_humidity( active_preset, min_soil_humidity );

                char *max_soil_humidity_string = extract_from_json( "MaxSoilHumidity", temp_payload );
                int max_soil_humidity = atoi( max_soil_humidity_string );
                preset_set_min_soil_humidity( active_preset, max_soil_humidity );

            } else {
                uart_send_string_blocking(USART_0, "3\n");
            }
        }
    }

    return 0;
}

char *extract_substring( char *start, char *end ) {
    size_t len = end - start;
    char *substr = malloc(len + 1);

    if (!substr) return NULL;

    memcpy(substr, start, len);
    substr[len] = '\0';

    return substr;
}

char *extract_from_json( char *to_extract, char *json ){
    char temp[50];
    sprintf( temp, "\"%s\"", to_extract );

    char *extracted_start = strstr( json, temp );
    extracted_start = strstr( extracted_start, ":" );

    ++extracted_start;
    char *extracted_end;

    if ( NULL != strstr( extracted_start, "," ) ) extracted_end = strstr( extracted_start, "," );
    else extracted_end = strstr( extracted_start, "}" );

    char *extracted = extract_substring( extracted_start, extracted_end );

    return extracted;

}

void loop1(){

    char *topic = "greenhouse/sensors";

    uint8_t humidity_int, humidity_dec, temperature_int, temperature_dec;
    uint16_t soil_humidity;

    dht11_get(&humidity_int, &humidity_dec, &temperature_int, &temperature_dec);

    soil_humidity = soil_read();

    uint16_t light_int = light_get_percentage();
    char payload[300] = "";
    // sprintf(payload, "{\"air_temperature\":%d,\"air_humidity\":%d,\"soil_humidity\":%d,\"light_level\":%d}", temperature_int, humidity_int, soil_humidity, light_int);
    sprintf(payload, "{\"MacAddress\":%s,\"SensorData\":[{\"Type\":\"Temperature\",\"Value\":%d,\"Unit\":\"C\",},{\"Type\":\"AirHumidity\",\"Value\":%d,\"Unit\":\"%%\",},{\"Type\":\"SoilHumidity\",\"Value\":%d,\"Unit\":\"%%\",},{\"Type\":\"Brightness\",\"Value\":%d,\"Unit\":\"%%\",}],\"Timestamp\":%s,}",
    mac_address, temperature_int, humidity_int, soil_humidity, light_int, "2025-05-09T11:45:00Z");
    if ( !preset_is_watering_manual( active_preset ) ){
        int min_soil_humidity = preset_get_min_soil_humidity( active_preset );
        int max_soil_humidity = preset_get_min_soil_humidity( active_preset );
        if( min_soil_humidity > soil_humidity ){
            leds_turnOn(4);
            pump_on();
            periodic_task_init_b( loop2, 5000 );
        }
        else if ( max_soil_humidity < soil_humidity ){
            leds_turnOff(4);
            pump_off();
        }
    }
    else {
        leds_turnOff(4);
        pump_off();
    }
    
    mqtt_publish( topic, payload, 0, 0, 0 );

}

void loop2(){

    uart_send_string_blocking(USART_0, "stopping pump\n");
    leds_turnOff(4);
    pump_off();

}