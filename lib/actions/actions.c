#include "periodic_task.h"
#include "leds.h"
#include "fertilizer.h"

static void pump_cut_off(){

    leds_turnOff(4);
    pump_off();

}

void actions_pump(){

    periodic_task_init_b( pump_cut_off, 5000 );

}

void actions_light(){

    leds_toggle(2);

}

void actions_fertilizer(){

    fertilizer_trigger();

}