
#include "unity.h"

#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <math.h>
#include "dht11.h"
#include <stdio.h>
#include "light.h"

void setUp(void)
{
}

void tearDown(void)
{
}


void test_dht11_status()
{
    dht11_init();
    _delay_ms(2000);
    TEST_ASSERT_EQUAL(DHT11_OK, dht11_get(NULL, NULL, NULL, NULL));
}

void test_temperature_is_22celc()
{

    dht11_init();
    _delay_ms(2000);
    uint8_t celc, celc_dec, hum_int, hum_dec;
    dht11_get(&hum_int, &hum_dec, &celc, &celc_dec);
    TEST_ASSERT_INT_WITHIN(5, 22, celc); // between 17 and

    char message[1024];
    sprintf(message, "INFO! dht11 measurement! Temperature %d.%d C! Humidity %d.%d%%       :1:_:PASS\n", celc, celc_dec, hum_int, hum_dec);
    TEST_MESSAGE(message); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message
}

void test_light_returns_a_value()
{
    light_init();
    _delay_ms(300);
    uint16_t light = light_read();
    TEST_ASSERT_INT16_WITHIN(400, 500, light);

    char message[1024];
    sprintf(message, "INFO! photo_resistor measurement= %d       :1:_:PASS\n", light);
    TEST_MESSAGE(message); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message
}

void test_light_is_dark()
{
    light_init();

    TEST_MESSAGE("INFO! 5 sec to make dark        :1:_:PASS\n"); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message
    _delay_ms(1000);
    TEST_MESSAGE("INFO! 4 sec to make dark        :1:_:PASS\n"); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message
    _delay_ms(1000);
    TEST_MESSAGE("INFO! 3 sec to make dark        :1:_:PASS\n"); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message
    _delay_ms(1000);
    TEST_MESSAGE("INFO! 2 sec to make dark        :1:_:PASS\n"); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message
    _delay_ms(1000);
    TEST_MESSAGE("INFO! 1 sec to make dark        :1:_:PASS\n"); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message
    _delay_ms(1000);
    uint16_t light = light_read();
    char message[1024];
    sprintf(message, "INFO! photo_resistor while dark measurement= %d       :1:_:PASS\n", light);
    TEST_MESSAGE(message); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message
    TEST_ASSERT_TRUE_MESSAGE((light > 5 && light < 300), "There should be dark, (light > 5 && light < 300)");
}

void test_light_is_is_light()
{
    light_init();

    TEST_MESSAGE("INFO! 2 sec to make light        :1:_:PASS\n"); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message
    _delay_ms(1000);
    TEST_MESSAGE("INFO! 1 sec to make light        :1:_:PASS\n"); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message
    _delay_ms(1000);

    uint16_t light = light_read();
    char message[1024];
    sprintf(message, "INFO! photo_resistor while light measurement= %d       :1:_:PASS\n", light);
    TEST_MESSAGE(message); // TEST_MESSAGE("m e s s a g e :1:_:PASS\n"); // no : in the message

    TEST_ASSERT_TRUE_MESSAGE((light < 300), "There should be light, but it's reading dark (light < 300)");
}


int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dht11_status);
    RUN_TEST(test_temperature_is_22celc);
    RUN_TEST(test_light_returns_a_value);
    RUN_TEST(test_light_is_is_light);
    return UNITY_END();
}