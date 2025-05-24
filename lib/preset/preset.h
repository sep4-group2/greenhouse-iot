#pragma once
#include <stdbool.h>


typedef enum {
    ACTION_MANUAL,
    ACTION_AUTOMATED
} ACTION_METHOD;

typedef struct preset *preset_t;

preset_t preset_init( );

void preset_destroy( preset_t self );

bool preset_is_watering_manual( preset_t self );

bool preset_is_lighting_manual( preset_t self );

bool preset_is_fertilizer_manual( preset_t self );

ACTION_METHOD preset_get_watering_method ( preset_t self );

ACTION_METHOD preset_get_lighting_method ( preset_t self );

ACTION_METHOD preset_get_fertilizer_method ( preset_t self );

int preset_get_min_soil_humidity ( preset_t self );

int preset_get_max_soil_humidity ( preset_t self );

int preset_get_light_hours( preset_t self );

void preset_set_watering_method( preset_t self, ACTION_METHOD method );

void preset_set_lighting_method( preset_t self, ACTION_METHOD method );

void preset_set_fertilizer_method( preset_t self, ACTION_METHOD method );

void preset_set_min_soil_humidity( preset_t self, int minHumidity );

void preset_set_max_soil_humidity( preset_t self, int maxHumidity );

void preset_set_light_hours( preset_t self, int hours );