#include "unity.h"
#include "soil.h"
#include <avr/io.h>

void setUp(void) {
    ADMUX = 0;
    ADCSRA = 0;
    ADCSRB = 0;
    DIDR2 = 0;
}

void tearDown(void) {}

void test_soil_init_should_configure_ADC_properly() {
    soil_init();
    TEST_ASSERT_TRUE(ADMUX & (1 << REFS0));
    TEST_ASSERT_TRUE(ADCSRA & (1 << ADEN));
    TEST_ASSERT_TRUE(DIDR2 & (1 << SOIL_ADC_CHANNEL));
}

void test_soil_read_should_return_reasonable_value() {
    soil_init();
    uint16_t value = soil_read();
    TEST_ASSERT_TRUE(value <= 100 && value >= 0);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_soil_init_should_configure_ADC_properly);
    RUN_TEST(test_soil_read_should_return_reasonable_value);
    return UNITY_END();
}
