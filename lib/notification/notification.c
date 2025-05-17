#include <stdbool.h>
#include <string.h>

#include "mqtt.h"
#include "clock.h"

extern char mac_address[18];

void notification_send( char *action, bool status ){

    char *topic = "greenhouse/action";
    char *status_text = (status ? "true" : "false" );
    char timestamp[21];
    clock_to_string( &global_clock, timestamp, sizeof(timestamp) );

    char payload[200] = "";

    strcat( payload, "{\"MacAddress\":\"");
    strcat( payload, mac_address);
    strcat( payload, "\",\"Command\":\"");
    strcat( payload, action);
    strcat( payload, "\",\"Status\":");
    strcat( payload, status_text);
    strcat( payload, "\",\"Timestamp\":\"");
    strcat( payload, timestamp);
    strcat( payload, "\"}");

    mqtt_publish( topic, payload, 0, 0, 0 );

}