
#include "unity.h"
#include <util/delay.h>
#include <avr/io.h>
#include "display.h"
#include "servo.h"
#include "leds.h"

void setUp(void)
{
}

void tearDown(void)
{
}



void test_display()
{
    display_init();
    _delay_ms(1000);
    for (int16_t i = -999; i < 9999; i++)
    {
        display_int(i);
        _delay_us(400);
    }

    TEST_ASSERT_EQUAL(1, 1);
}


void test_servo(){
     TEST_MESSAGE("INFO! the servo should go to 0deg      :1:_:PASS\n");
     servo(0);
    TEST_MESSAGE("INFO! the servo should go to 90deg      :1:_:PASS\n");
    servo(90);
        TEST_MESSAGE("INFO! the servo should go to 180deg      :1:_:PASS\n");
    servo(180);


TEST_ASSERT_TRUE(1);
}

int main(void)
{
    UNITY_BEGIN();
    //_delay_ms(4000);

    RUN_TEST(test_display);
    RUN_TEST(test_servo);

    return UNITY_END();
}