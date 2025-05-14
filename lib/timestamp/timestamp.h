#pragma once

#include <stdint.h>
#include <stdbool.h>

void timestamp_init(void);
void timestamp_tick(void);
void timestamp_set(uint8_t h, uint8_t m, uint8_t s);
//void timestamp_set(uint16_t y, uint8_t mo, uint8_t d, uint8_t h, uint8_t m, uint8_t s);
void timestamp_get(uint8_t *h, uint8_t *m, uint8_t *s);
bool timestamp_sync_via_http(void); 
void http_response_callback(uint8_t byte);
