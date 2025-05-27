#include "../fff.h"
#include "unity.h"
#include "light.h"

#include <stdint.h>


uint8_t ADMUX;
uint8_t ADCSRA;
uint8_t ADCSRB;
uint8_t ADCL;
uint8_t ADCH;
uint8_t DIDR2;
uint8_t PORTK;
uint8_t DDRK;


void light_init(void) {
    ADMUX = 0x47;
    ADCSRA = 0x87;
    DIDR2 = 0x80;  
}

uint8_t light_get_percentage(void) {
    uint16_t raw = ADCL | (ADCH << 8);
    uint16_t inverted = 1023 - raw;
    return (inverted * 100) / 1023;
}

void setUp(void) {
    ADMUX = 0;
    ADCSRA = 0;
    ADCSRB = 0;
    ADCL = 0;
    ADCH = 0;
    DIDR2 = 0;
}
void tearDown(void) {}

void test_pc_comm_default_callback_func_is_null()
{
    light_init();
    TEST_ASSERT_EQUAL(71, ADMUX);
}

void test_light_get_percentage_bounds(void) {
    // ADC = 0= 100%
    ADCL = 0x00;
    ADCH = 0x00;
    TEST_ASSERT_EQUAL_UINT8(100, light_get_percentage());

    // ADC =1023 = 0%
    ADCL = 0xFF;
    ADCH = 0x03;
    TEST_ASSERT_EQUAL_UINT8(0, light_get_percentage());
}


int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_pc_comm_default_callback_func_is_null);
    RUN_TEST(test_light_get_percentage_bounds);
    return UNITY_END();
}
