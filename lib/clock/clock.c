#include "clock.h"
#include "uart.h"
#include <stdio.h>

Clock global_clock;

static bool is_leap_year(uint16_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}


static uint8_t days_in_month(uint8_t month, uint16_t year) {
    static const uint8_t days[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (month == 2 && is_leap_year(year)) return 29;
    return days[month - 1];
}

void clock_init(Clock *clk, uint16_t y, uint8_t mo, uint8_t d, uint8_t h, uint8_t mi, uint8_t s) {
    clk->year = y;
    clk->month = mo;
    clk->day = d;
    clk->hour = h;
    clk->minute = mi;
    clk->second = s;
}

void clock_tick(Clock *clk) {
    clk->second++;
    if (clk->second >= 60) {
        clk->second = 0;
        clk->minute++;
        if (clk->minute >= 60) {
            clk->minute = 0;
            clk->hour++;
            if (clk->hour >= 24) {
                clk->hour = 0;
                clk->day++;
                if (clk->day > days_in_month(clk->month, clk->year)) {
                    clk->day = 1;
                    clk->month++;
                    if (clk->month > 12) {
                        clk->month = 1;
                        clk->year++;
                    }
                }
            }
        }
    }
}

void clock_to_string(const Clock *clk, char *buffer, size_t size) {
    snprintf(buffer, size, "%04u-%02u-%02uT%02u:%02u:%02uZ\0",
        clk->year, clk->month, clk->day,
        clk->hour, clk->minute, clk->second);
}

void clock_update_task(void) {
    clock_tick(&global_clock);
}
