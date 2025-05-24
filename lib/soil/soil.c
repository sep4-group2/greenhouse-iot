/**
 * @file soil.c
 * @brief Driver for Capacitive Soil Moisture Sensor v1.2
 *
 * This file provides the implementation for initializing and reading the value
 * from a Capacitive Soil Moisture Sensor connected to pin SOIL_PIN on the ATmega2560.
 *
 * @date 09-04-2025
 * @author Erland Larsen, VIA University College
 */
#include "soil.h"

#ifdef __AVR__
#include <avr/io.h>

#define SOIL_DRY 480
#define SOIL_WET 180

/**
 * @brief Initialize ADC for Soil Moisture sensor
 *
 * This function initializes the ADC to read values from the Soil Moisture sensor
 * connected to pin SOIL_PIN.
 */
void soil_init()
{

    // Set reference voltage to AVCC
    ADMUX = (1 << REFS0);
    // Enable ADC and set prescaler to 128 (16MHz/128 = 125kHz)
    // ADC must operate between 50kHz and 200kHz for its full 10-bit resolution
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1)| (1 << ADPS0);
    

    // Disable digital input on SOIL_ADC_CHANNEL (page 287)
    // This will reduce power consumption on the pin
    DIDR2 = (1 << SOIL_ADC_CHANNEL);
}

/**
 * @brief Read value from Soil Moisture sensor
 *
 * This function reads the ADC values from the Soil Moisture sensor
 * connected to pin SOIL_PIN.
 * 
 * @return 10-bit ADC value read from the Soil Moisture sensor
 */
uint16_t soil_read()
{
    uint32_t timeout = 40000;//if 2cc for incrementing and evaluation the timeout is 5ms
    // The  MUX0:5 should be set to 100000 for choosing ADC8 (look at page 283)
    ADMUX &= ~((1<<MUX4)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0));
    ADCSRB |= (1<<MUX5);

    // Start the conversion
    ADCSRA |= (1 << ADSC);

    // Wait for the conversion to complete
    while ((ADCSRA & (1 << ADSC))&& timeout > 0){timeout--;};

    // Read the 10-bit ADC value
    uint16_t adc_value = ADC;

    float percentage = 100.0 * (adc_value - SOIL_WET) / (SOIL_DRY - SOIL_WET);

    if( 0.0 > percentage ) percentage = 0.0;
    else if( 100.0 < percentage ) percentage = 100.0;

    return 100.0 - percentage;
}

#endif