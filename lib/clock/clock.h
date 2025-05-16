#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} Clock;

extern Clock global_clock;

void clock_init(Clock *clk, uint16_t y, uint8_t mo, uint8_t d, uint8_t h, uint8_t mi, uint8_t s);
void clock_tick(Clock *clk);
void clock_to_string(const Clock *clk, char *buffer, size_t size);
void clock_update_task(void);

#endif
