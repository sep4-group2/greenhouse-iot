#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stdint.h>
#include <stdbool.h>

void timestamp_init(void);
void timestamp_get(uint8_t *h, uint8_t *m, uint8_t *s);
void timestamp_set(uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y);
void timestamp_get_date(uint8_t *d, uint8_t *mo, uint16_t *y);
bool timestamp_sync_via_http(void);

#endif
