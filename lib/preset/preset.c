#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "preset.h"

typedef struct preset {

    ACTION_METHOD watering_method;
    ACTION_METHOD lighting_method;
    ACTION_METHOD fertilizer_method;

    int minSoilHumidity;
    int maxSoilHumidity;
    int lightHours;

} preset;

preset_t preset_init( ){
    preset_t _new_preset = calloc( 1, sizeof( preset ) );

    if( NULL == _new_preset ) return NULL;

    _new_preset->watering_method = ACTION_MANUAL;
    _new_preset->fertilizer_method = ACTION_MANUAL;
    _new_preset->lighting_method = ACTION_MANUAL;

    _new_preset->minSoilHumidity = 0;
    _new_preset->maxSoilHumidity = 0;
    _new_preset->lightHours = 0;
    
    return _new_preset;
}

void preset_destroy( preset_t self ){
    if( NULL == self ) return;
    free(self);
}

bool preset_is_watering_manual( preset_t self ){
    return ACTION_MANUAL == self->watering_method;
}

bool preset_is_lighting_manual( preset_t self ){
    return ACTION_MANUAL == self->lighting_method;
}

bool preset_is_fertilizer_manual( preset_t self ){
    return ACTION_MANUAL == self->fertilizer_method;
}

ACTION_METHOD preset_get_watering_method ( preset_t self ){
    return self->watering_method;
}

ACTION_METHOD preset_get_lighting_method ( preset_t self ){
    return self->lighting_method;
}

ACTION_METHOD preset_get_fertilizer_method ( preset_t self ){
    return self->fertilizer_method;
}

int preset_get_min_soil_humidity ( preset_t self ){
    return self->minSoilHumidity;
}

int preset_get_max_soil_humidity ( preset_t self ){
    return self->maxSoilHumidity;
}

int preset_get_light_hours( preset_t self ){
    return self->lightHours;
}

void preset_set_watering_method( preset_t self, ACTION_METHOD method ) {
    self->watering_method = method;
}

void preset_set_lighting_method( preset_t self, ACTION_METHOD method ) {
    self->lighting_method = method;
}

void preset_set_fertilizer_method( preset_t self, ACTION_METHOD method ) {
    self->fertilizer_method = method;
}

void preset_set_min_soil_humidity( preset_t self, int minHumidity ) {
    self->minSoilHumidity = minHumidity;
}

void preset_set_max_soil_humidity( preset_t self, int maxHumidity ) {
    self->maxSoilHumidity = maxHumidity;
}

void preset_set_light_hours( preset_t self, int hours ) {
    self->lightHours = hours;
}
