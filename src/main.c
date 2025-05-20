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
#include "lightbulb.h"
#include "dht11.h"
#include "soil.h"
#include "pump.h"
#include "preset.h"
#include "fertilizer.h"
#include "actions.h"

#include "mqtt_topics.h"
#include "mqtt_received_packet.h"
#include "mqtt_buffer.h"
#include "MQTTPacket.h"
#include "timestamp.h"
#include "clock.h"

extern volatile mqtt_buffer_t mqtt_packet_buffer;

static preset_t active_preset;
static bool water_pump_trigger = false;

static int cycle = 0;
static int light_cycle = 0;
static int dark_cycle = 0;

char mac_address[18];

char *extract_substring( char *start, char *end );

char *extract_from_json( char *to_extract, char *json );

static void process_single_publish ( char *publish_topic, char *publish_payload );

static void process_single_packet( unsigned char packet_type, char* buf, int len );

static void get_topic_with_address( char *topic_dest, char *topic );

void loop();

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
    lightbulb_init();
    display_init();
    timestamp_init();

    sei();

    char *ssid = "Xiaomi 12";
    char *password = "patty123";
    char *mqtt_ip = "192.168.139.178";
    int mqtt_port = 1883;

    // if (wifi_command_set_mode_to_1() != WIFI_OK ||
    //     wifi_command_disable_echo() != WIFI_OK ||
    //     wifi_command_join_AP(ssid, password) != WIFI_OK) {
    //     uart_send_string_blocking(USART_0, "Error WiFi\n");
    //     while(1);
    // }

    // if (!timestamp_sync_via_http()) {
    //     uart_send_string_blocking(USART_0, "Error HTTP\n");
    // }

    // wifi_command_close_TCP_connection();


    // uint8_t hour = 0, minute = 0, second = 0;
    // uint8_t day_ = 0, month_ = 0;
    // uint16_t year_ = 0;

    // timestamp_get(&hour, &minute, &second);
    // timestamp_get_date(&day_, &month_, &year_);
    // clock_init(&global_clock, year_, month_, day_, hour, minute, second);

    // periodic_task_init_c( clock_update_task, 1000);

    mqtt_connect(ssid, password, mqtt_ip, mqtt_port, mac_address);

    periodic_task_init_a( loop, 10000 );

    while (1) {

        if ( !mqtt_buffer_is_empty( mqtt_packet_buffer ) ) {

            mqtt_received_packet_t temp = mqtt_buffer_pop ( mqtt_packet_buffer );

            char packet_type = mqtt_received_packet_get_type( temp );
            char *buf = mqtt_received_packet_get_buf( temp );
            int len = mqtt_received_packet_get_len( temp );

            process_single_packet( packet_type, buf, len );

        }
    }

    return 0;
}

static void get_topic_with_address( char *topic_dest, char *topic ){

    char temp_topic[50] = "greenhouse/";
    strcat(temp_topic, mac_address);
    strcat(temp_topic, "/");
    strcat(temp_topic, topic);
    strcpy( topic_dest, temp_topic);
    
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
    char temp[100];
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

void loop(){

    char *topic = "greenhouse/sensors";

    uint8_t humidity_int, humidity_dec, temperature_int, temperature_dec;
    uint16_t soil_humidity;

    dht11_get(&humidity_int, &humidity_dec, &temperature_int, &temperature_dec);

    soil_humidity = soil_read();

    uint16_t light_int = light_get_percentage();
    char payload[300] = "";

    sprintf(payload, "{\"MacAddress\":%s,\"SensorData\":[{\"Type\":\"Temperature\",\"Value\":%d,\"Unit\":\"C\",},{\"Type\":\"AirHumidity\",\"Value\":%d,\"Unit\":\"%%\",},{\"Type\":\"SoilHumidity\",\"Value\":%d,\"Unit\":\"%%\",},{\"Type\":\"Brightness\",\"Value\":%d,\"Unit\":\"%%\",}],\"Timestamp\":%s,}",
    mac_address, temperature_int, humidity_int, soil_humidity, light_int, "2025-05-09T11:45:00Z");
    
    if ( !preset_is_watering_manual( active_preset ) ){

        int min_soil_humidity = preset_get_min_soil_humidity( active_preset );
        int max_soil_humidity = preset_get_min_soil_humidity( active_preset );

        if( min_soil_humidity > soil_humidity ){
            water_pump_trigger = true;
        }
        else if ( max_soil_humidity < soil_humidity ){
            water_pump_trigger = false;
        }

        if(water_pump_trigger){
            actions_pump();
        }

    }
    else {
        leds_turnOff(4);
        pump_off();
        water_pump_trigger = false;
    }

    if ( !preset_is_lighting_manual( active_preset ) ){

        if( 0 < cycle ){

            cycle--;

            if( 50 < light_int ){

                if( 0 == light_cycle ){

                    actions_light_off();

                }
                else{
                    --light_cycle;
                }
                
            }
            else {

                if( 0 == dark_cycle ){

                    actions_light_on(2);

                }
                else{
                    --dark_cycle;
                }

            }

        }
        else {
            cycle = 96;
            light_cycle = preset_get_light_hours( active_preset ) * 4;
            dark_cycle = cycle - light_cycle;
        }

    }
    
    mqtt_publish( topic, payload, 0, 0, 0 );

}

static void process_single_publish ( char *publish_topic, char *publish_payload ){

    while( NULL != strstr( publish_topic, "/" ) ){
        publish_topic = strstr( publish_topic, "/" );
        publish_topic++;
    }
            
    if (strcmp(publish_topic, "light") == 0) {

        actions_light_toggle();

    } else if (strcmp(publish_topic, "watering") == 0) {

        actions_pump();

    } else if (strcmp(publish_topic, "fertilizer") == 0) {

        actions_fertilizer();

    } else if (strcmp(publish_topic, "preset") == 0) {

        char *watering_method = extract_from_json( "WateringMethod", publish_payload );

        if( 0 == strcmp( watering_method, "\"manual\"" ) ) preset_set_watering_method( active_preset, ACTION_MANUAL );
        else if( 0 == strcmp( watering_method, "\"automated\"" ) ) {

            preset_set_watering_method( active_preset, ACTION_AUTOMATED );

            char *min_soil_humidity_string = extract_from_json( "MinSoilHumidity", publish_payload );
            int min_soil_humidity = atoi( min_soil_humidity_string );
            preset_set_min_soil_humidity( active_preset, min_soil_humidity );

            char *max_soil_humidity_string = extract_from_json( "MaxSoilHumidity", publish_payload );
            int max_soil_humidity = atoi( max_soil_humidity_string );
            preset_set_min_soil_humidity( active_preset, max_soil_humidity );

        }

        char *lighting_method = extract_from_json( "LightingMethod", publish_payload );

        if( 0 == strcmp( lighting_method, "\"manual\"" ) ) preset_set_lighting_method( active_preset, ACTION_MANUAL );
        else if( 0 == strcmp( lighting_method, "\"automated\"" ) ) {

            preset_set_lighting_method( active_preset, ACTION_AUTOMATED );

            char *hours_of_light_string = extract_from_json( "HoursOfLight", publish_payload );
            int hours_of_light = atoi( hours_of_light_string );
            preset_set_light_hours( active_preset, hours_of_light );

            cycle = 96;
            light_cycle = hours_of_light * 4;
            dark_cycle = cycle - light_cycle;

        }


    } else {
        uart_send_string_blocking(USART_0, "3\n");
    }
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
    int payload_len;


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
            &topic, &payload, &payload_len,
            buf, len
        ) == 1 )
        {

            char *topic_copy = malloc(topic.lenstring.len + 1);
            if (topic_copy != NULL) {
                memcpy(topic_copy, topic.lenstring.data, topic.lenstring.len);
                topic_copy[topic.lenstring.len] = '\0';
            }

                char publish_payload[128] = "";

            uart_send_string_blocking( USART_0, topic_copy );


                strncpy( publish_payload, payload, payload_len );

                process_single_publish( topic_copy, publish_payload );

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