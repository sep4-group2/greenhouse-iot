#ifdef EXCLUDE_UART

#include "uart_packet.h"
#include "uart_buffer.h"
#include "uart.h"
#include <stdint.h>
#include <stddef.h>

void uart_init(USART_t usart, uint32_t baudrate, UART_Callback_t callback) {}
void uart_send_blocking(USART_t usart,uint8_t data) {}
void uart_send_array_nonBlocking(USART_t usart,  uint8_t *str, uint16_t len) {}
void uart2_send_array_nonblocking(uint8_t *data, uint16_t length) {}
void uart_send_array_blocking(USART_t usart, uint8_t *data, uint16_t length) {}
void uart_send_string_blocking(USART_t usart, char *data) {}
UART_Callback_t uart_get_rx_callback(USART_t usart) { return NULL; }

#else

#ifndef EXCLUDE_UART
#include "uart_packet.h"
#include "uart_buffer.h"
#include "uart.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <string.h>

#include "includes.h"

static UART_Callback_t usart0_rx_callback = NULL;
static UART_Callback_t usart1_rx_callback = NULL;
static UART_Callback_t usart2_rx_callback = NULL;
static UART_Callback_t usart3_rx_callback = NULL;

UART_Callback_t uart_get_rx_callback(USART_t usart){
    switch (usart)
    {
    case USART_0:
        return usart0_rx_callback;
    case USART_1:
        return usart1_rx_callback;
    case USART_2:
        return usart2_rx_callback;
    case USART_3:
        return usart3_rx_callback;
    default:
        return 0;
    }
}

#ifndef WINDOWS_TEST
#ifndef TARGET_TEST
ISR(USART0_RX_vect) {
    if (usart0_rx_callback != NULL)
        usart0_rx_callback(UDR0);
}
#endif

ISR(USART1_RX_vect) {
    if (usart1_rx_callback != NULL)
        usart1_rx_callback(UDR1);
}

ISR(USART2_RX_vect) {
    if (usart2_rx_callback != NULL)
        usart2_rx_callback(UDR2);
}

ISR(USART3_RX_vect) {
    if (usart3_rx_callback != NULL)
        usart3_rx_callback(UDR3);
}
#endif

inline static void uart_init_usart0(uint16_t ubrr, UART_Callback_t callback) {
    UCSR0B = (1 << TXEN0) | (1<< RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    if (callback != NULL) {
        usart0_rx_callback = callback;
        UCSR0B |= (1 << RXCIE0);
    }
}

inline static void uart_init_usart1(uint16_t ubrr, UART_Callback_t callback) {
    UCSR1B = (1 << TXEN1) | (1 << RXEN1);
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
    UBRR1H = (uint8_t)(ubrr >> 8);
    UBRR1L = (uint8_t)ubrr;

    if (callback != NULL) {
        usart1_rx_callback = callback;
        UCSR1B |= (1 << RXCIE1);
    }
}

inline static void uart_init_usart2(uint16_t ubrr, UART_Callback_t callback) {
    UCSR2B = (1 << TXEN2) | (1 << RXEN2);
    UCSR2C = (1 << UCSZ21) | (1 << UCSZ20);
    UBRR2H = (uint8_t)(ubrr >> 8);
    UBRR2L = (uint8_t)ubrr;

    if (callback != NULL) {
        usart2_rx_callback = callback;
        UCSR2B |= (1 << RXCIE2);
    }
}

inline static void uart_init_usart3(uint16_t ubrr, UART_Callback_t callback) {
    UCSR3B = (1 << TXEN3) | (1 << RXEN3);
    UCSR3C = (1 << UCSZ31) | (1 << UCSZ30);
    UBRR3H = (uint8_t)(ubrr >> 8);
    UBRR3L = (uint8_t)ubrr;

    if (callback != NULL) {
        usart3_rx_callback = callback;
        UCSR3B |= (1 << RXCIE3);
    }
}

void uart_init(USART_t usart, uint32_t baudrate, UART_Callback_t callback) {
    cli();
    uint16_t ubrr = (F_CPU / (16ULL * baudrate)) - 1;
    if (ubrr == 7) ubrr = 8;

    switch (usart) {
        case USART_0: uart_init_usart0(ubrr, callback); break;
        case USART_1: uart_init_usart1(ubrr, callback); break;
        case USART_2: uart_init_usart2(ubrr, callback); break;
        case USART_3: uart_init_usart3(ubrr, callback); break;
        default: break;
    }
    sei();
}

void uart_send_blocking(USART_t usart, uint8_t data) {
    switch (usart) {
        case USART_0: while (!(UCSR0A & (1 << UDRE0))); UDR0 = data; break;
        case USART_1: while (!(UCSR1A & (1 << UDRE1))); UDR1 = data; break;
        case USART_2: while (!(UCSR2A & (1 << UDRE2))); UDR2 = data; break;
        case USART_3: while (!(UCSR3A & (1 << UDRE3))); UDR3 = data; break;
        default: break;
    }
}

void uart_send_array_blocking(USART_t usart, uint8_t *data, uint16_t length) {
    for (uint16_t i = 0; i < length; i++)
        uart_send_blocking(usart, data[i]);
}

void uart_send_string_blocking(USART_t usart, char *data) {
    uart_send_array_blocking(usart, (uint8_t*)data, strlen(data));
}

static volatile uint8_t *usart0_transmit_buffer;
static volatile uint16_t usart0_transmit_index;
static volatile uint16_t usart0_transmit_length;
static volatile uint8_t usart0_transmission_in_progress = 0;

static volatile uint8_t *usart1_transmit_buffer;
static volatile uint16_t usart1_transmit_index;
static volatile uint16_t usart1_transmit_length;
static volatile uint8_t usart1_transmission_in_progress = 0;

static volatile uint8_t *usart2_transmit_buffer;
static volatile uint16_t usart2_transmit_index;
static volatile uint16_t usart2_transmit_length;
volatile uint8_t usart2_transmission_in_progress = 0;
volatile uart_packet_t current_pkt = NULL;

static volatile uint8_t *usart3_transmit_buffer;
static volatile uint16_t usart3_transmit_index;
static volatile uint16_t usart3_transmit_length;
static volatile uint8_t usart3_transmission_in_progress = 0;


void uart_send_array_nonBlocking(USART_t usart, uint8_t *str, uint16_t len) {
    switch(usart) {
        case USART_0: while (usart0_transmission_in_progress); break;
        case USART_1: while (usart1_transmission_in_progress); break;
        case USART_2: while (usart2_transmission_in_progress); break;
        case USART_3: while (usart3_transmission_in_progress); break;
    }

    cli();
    switch (usart) {
        case USART_0:
            usart0_transmit_buffer = str;
            usart0_transmit_index = 0;
            usart0_transmit_length = len;
            usart0_transmission_in_progress = 1;
            UCSR0B |= (1 << UDRIE0);
            break;
        case USART_1:
            usart1_transmit_buffer = str;
            usart1_transmit_index = 0;
            usart1_transmit_length = len;
            usart1_transmission_in_progress = 1;
            UCSR1B |= (1 << UDRIE1);
            break;
        case USART_2:
            usart2_transmit_buffer = str;
            usart2_transmit_index = 0;
            usart2_transmit_length = len;
            usart2_transmission_in_progress = 1;
            UCSR2B |= (1 << UDRIE2);
            break;
        case USART_3:
            usart3_transmit_buffer = str;
            usart3_transmit_index = 0;
            usart3_transmit_length = len;
            usart3_transmission_in_progress = 1;
            UCSR3B |= (1 << UDRIE3);
            break;
        default: break;
    }
    sei();
}

#ifndef WINDOWS_TEST
#ifndef TARGET_TEST
ISR(USART0_UDRE_vect) {
    if (usart0_transmit_index < usart0_transmit_length)
        UDR0 = usart0_transmit_buffer[usart0_transmit_index++];
    else {
        UCSR0B &= ~(1 << UDRIE0);
        usart0_transmission_in_progress = 0;
    }
}
#endif

ISR(USART1_UDRE_vect) {
    if (usart1_transmit_index < usart1_transmit_length)
        UDR1 = usart1_transmit_buffer[usart1_transmit_index++];
    else {
        UCSR1B &= ~(1 << UDRIE1);
        usart1_transmission_in_progress = 0;
    }
}

ISR(USART2_UDRE_vect)
{
    if (usart2_transmit_index < usart2_transmit_length)
    {
        UDR2 = usart2_transmit_buffer[usart2_transmit_index++];
    }
    else
    {
        UCSR2B &= ~(1 << UDRIE2);
        usart2_transmission_in_progress = 0;
        
        usart2_transmit_buffer = NULL;
        usart2_transmit_index = 0;
        usart2_transmit_length = 0;

        if (current_pkt != NULL) {
            uart_packet_free(current_pkt);
            current_pkt = NULL;
        }
    }
}

ISR(USART3_UDRE_vect) {
    if (usart3_transmit_index < usart3_transmit_length)
        UDR3 = usart3_transmit_buffer[usart3_transmit_index++];
    else {
        UCSR3B &= ~(1 << UDRIE3);
        usart3_transmission_in_progress = 0;
    }
}
#endif

#endif // EXCLUDE_UART

#endif 
