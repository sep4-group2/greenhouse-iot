#include "wifi.h"
#include "uart.h"
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "light.h"
#include "display.h"
#include "pump.h"
#include "Buttons.h"

static uint8_t _buff[100];
static uint8_t _index = 0;
volatile static bool _done = false;
void console_rx(uint8_t _rx)
{
    uart_send_blocking(USART_0, _rx);
    if(('\r' != _rx) && ('\n' != _rx))
    {
        if(_index < 100-1)
        {
            _buff[_index++] = _rx;
        }
    }
    else
    {
        _buff[_index] = '\0';
        _index = 0;
        _done = true;
        uart_send_blocking(USART_0, '\n');
//        uart_send_string_blocking(USART_0, (char*)_buff);
    }
}

int main()
{
    // char welcome_text[] = "Welcome from SEP4 IoT hardware!\n";
    // char prompt_text[] = "Type text to send: ";

    uart_init(USART_0, 9600, console_rx);
    // wifi_init();
    // light_init();
    // display_init();

    pump_init();
    buttons_init();

    sei();

    // pump_on();

    // _delay_ms(5000);

    // pump_off();


    while(1)
    {
        if( 1 == buttons_1_pressed() ){
            pump_on();
        }
        if( 1 == buttons_2_pressed() ){
            pump_off();
        }
    _delay_us(500);

        // uint8_t brightness = light_get_percentage();

        //char temp[20];
        //sprintf(temp, "light: %d percent", brightness);

        //uart_send_string_blocking(USART_0,temp );
       // uint16_t brightness = light_read();
        // display_int(brightness); 
       // _delay_ms(1);
    }
    return 0;
}