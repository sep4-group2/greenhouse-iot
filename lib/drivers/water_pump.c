#include "water_pump.h"
#include "includes.h"

#define PUMP_BIT PC7
#define PUMP_DDR DDRC
#define PUMP_PORT PORTC
#define PUMP_PIN PINC

void water_pump_init(void) {
    PUMP_DDR |= (1 << PUMP_BIT);     
    PUMP_PORT &= ~(1 << PUMP_BIT);  
}

void water_pump_on(void) {
    PUMP_PORT |= (1 << PUMP_BIT);   
}

void water_pump_off(void) {
    PUMP_PORT &= ~(1 << PUMP_BIT);  
}

void water_pump_toggle(void) {
    PUMP_PORT ^= (1 << PUMP_BIT); 
}
