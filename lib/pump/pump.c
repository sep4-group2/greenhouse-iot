#include "pump.h"
#include "includes.h"

#ifdef __AVR__
#include <avr/io.h>


#define PUMP_BIT PC7
#define PUMP_DDR DDRC
#define PUMP_PORT PORTC
#define PUMP_PIN PINC

void pump_init(void) {
    PUMP_DDR |= (1 << PUMP_BIT);     
    PUMP_PORT &= ~(1 << PUMP_BIT);  
}

void pump_on(void) {
    PUMP_PORT |= (1 << PUMP_BIT);   
}

void pump_off(void) {
    PUMP_PORT &= ~(1 << PUMP_BIT);  
}

void pump_toggle(void) {
    PUMP_PORT ^= (1 << PUMP_BIT); 
}

#endif
