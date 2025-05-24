#pragma once

#include <stdint.h>


void pump_init(void);

void pump_on(void);

void pump_off(void);

void pump_toggle(void);

#ifdef __AVR__
#include <avr/io.h>
#endif