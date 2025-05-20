#include "unity.h"
#include "uart.h"
#include "../fff.h"

#ifndef WINDOWS_TEST
#include <avr/io.h>

DEFINE_FFF_GLOBALS;

// Fake ISR dependencies (optional if ISR is excluded in win)
FAKE_VOID_FUNC(cli);
FAKE_VOID_FUNC(sei);

void dummy_callback(uint8_t data) {}

// Reset before each test
void setUp(void) {
    RESET_FAKE(cli);
    RESET_FAKE(sei);
}

void tearDown(void) {}

void test_uart_get_rx_callback_returns_correct_callback() {
    // Arrange
    uart_init(USART_0, 9600, dummy_callback);
    // Act
    UART_Callback_t cb = uart_get_rx_callback(USART_0);
    // Assert
    TEST_ASSERT_EQUAL_PTR(dummy_callback, cb);
}

void test_uart_get_rx_callback_returns_null_for_invalid() {
    // Act
    UART_Callback_t cb = uart_get_rx_callback(99);  // invalid USART
    // Assert
    TEST_ASSERT_NULL(cb);
}

void test_uart_init_calls_cli_sei() {
    // Act
    uart_init(USART_0, 9600, dummy_callback);
    // Assert
    TEST_ASSERT_EQUAL(1, cli_fake.call_count);
    TEST_ASSERT_EQUAL(1, sei_fake.call_count);
}
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_uart_get_rx_callback_returns_correct_callback);
    RUN_TEST(test_uart_get_rx_callback_returns_null_for_invalid);
    RUN_TEST(test_uart_init_calls_cli_sei);
    return UNITY_END();
}
#endif
