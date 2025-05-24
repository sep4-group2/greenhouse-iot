#pragma once

#include <stdint.h>



void lightbulb_init(void);

void lightbulb_on(void);

void lightbulb_off(void);

void lightbulb_toggle(void);

#ifdef __AVR__
#include <avr/io.h>
#endif