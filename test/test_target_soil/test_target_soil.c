#include "unity.h"
#include "soil.h"

void setUp(void) {
    ADMUX = 0;
    ADCSRA = 0;
    ADCSRB = 0;
    DIDR2 = 0;
}

void tearDown(void) {}

void test_soil_init_should_configure_ADC_registers_correctly(void) {
    // Act
    soil_init();
    // Assert
    TEST_ASSERT_TRUE(ADMUX & (1 << REFS0));
    TEST_ASSERT_TRUE(ADCSRA & (1 << ADEN));
    TEST_ASSERT_TRUE(DIDR2 & (1 << SOIL_ADC_CHANNEL));
}

void test_soil_read_should_return_percentage_in_valid_range(void) {
    // Arrange
    soil_init();
    // Act
    uint16_t value = soil_read();
    // Assert
    TEST_ASSERT_TRUE(value >= 0 && value <= 100);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_soil_init_should_configure_ADC_registers_correctly);
    RUN_TEST(test_soil_read_should_return_percentage_in_valid_range);
    return UNITY_END();
}
