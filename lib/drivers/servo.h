#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include <avr/io.h>

#define DDR_SERVO DDRE
#define PORT_SERVO PORTE
#define P_SERVO PE3

void servo(uint8_t angle);

#endif 