
#include <stdbool.h>
#include "includes.h"
#include "periodic_task.h"
#include "pump.h"
#include "leds.h"
#include "fertilizer.h"
#include "lightbulb.h"
#include "notification.h"

bool light = false;
bool pump = false;
bool fertilizer = false;

static void pump_cut_off(){

    leds_turnOff(4);
    pump_off();
    notification_send( "Watering", false );

}

void actions_pump(){

    leds_turnOn(4);
    pump_on();
    periodic_task_init_b( pump_cut_off, 5000 );

}

void actions_light_toggle(){

    leds_toggle(2);
    lightbulb_toggle();
    light = !light;
    notification_send( "Lighting", light );

}

void actions_light_off(){

    lightbulb_off();
    leds_turnOff(2);
    light = false;
    notification_send( "Lighting", light );

}

void actions_light_on(){

    lightbulb_on();
    leds_turnOn(2);
    light = true;
    notification_send( "Lighting", light );

}

void actions_fertilizer(){

    fertilizer_trigger();
    notification_send( "Fertilizer", false );

}

