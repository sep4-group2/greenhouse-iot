#include "lightbulb.h"
#include "includes.h"

#ifdef __AVR__
#include <avr/io.h>

#define LIGHTBULB_BIT PA7
#define LIGHTBULB_DDR DDRA
#define LIGHTBULB_PORT PORTA
#define LIGHTBULB_PIN PINA

void lightbulb_init(void) {
    LIGHTBULB_DDR |= (1 << LIGHTBULB_BIT);     
    LIGHTBULB_PORT &= ~(1 << LIGHTBULB_BIT);  
}

void lightbulb_on(void) {
    LIGHTBULB_PORT |= (1 << LIGHTBULB_BIT);   
}

void lightbulb_off(void) {
    LIGHTBULB_PORT &= ~(1 << LIGHTBULB_BIT);  
}

void lightbulb_toggle(void) {
    LIGHTBULB_PORT ^= (1 << LIGHTBULB_BIT); 
}

#endif