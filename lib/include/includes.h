
#ifndef WINDOWS_TEST
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#else
#include "mock_avr_io.h"
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>