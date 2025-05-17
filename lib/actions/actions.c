#include <stdbool.h>

#include "periodic_task.h"
#include "pump.h"
#include "leds.h"
#include "fertilizer.h"
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

    periodic_task_init_b( pump_cut_off, 5000 );

}

void actions_light(){

    leds_toggle(2);
    light = !light;
    notification_send( "Lighting", light );

}

void actions_fertilizer(){

    fertilizer_trigger();
    notification_send( "Fertilizer", false );

}