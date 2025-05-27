/**
 * @file periodic_task.c
 * @brief Periodic Task Driver implementation for ATmega2560
 *
 * This file provides the implementation for initializing Timer0 and attaching user functions
 * that will be called at different frequencies.
 *
 * @author Your Name
 * @date September 2023
 */

#include "periodic_task.h"
#include "includes.h"


static void (*user_func_a)(void);  // Pointer to first user function
static void (*user_func_b)(void);  // Pointer to second user function
static void (*user_func_c)(void);  // Pointer to third user function
static void (*user_func_d)(void);  // Pointer to fourth user function!

void periodic_task_disable_b();

static uint16_t ocr3_value = 0;
static uint16_t ocr3c_value = 0;
static uint16_t ocr4_value = 0;
static uint16_t ocr5_value = 0;

static uint16_t cnt_a = 0;
static uint16_t cnt_b = 0;
static uint16_t cnt_c = 0;
static uint16_t cnt_d = 0;

static uint16_t loops_a = 0;
static uint16_t loops_b = 0;
static uint16_t loops_c = 0;
static uint16_t loops_d = 0;


// Timer0 Compare Match A interrupt service routine
#ifndef WINDOWS_TEST
ISR(TIMER3_COMPA_vect) {
    if (cnt_a==0)
    {
        OCR3A=OCR3A+ocr3_value;
        cnt_a=loops_a;
        user_func_a();
        
    }
    else{
        cnt_a--;
    }
}

ISR(TIMER4_COMPB_vect) {
    if (cnt_b==0)
    {

        OCR4B=OCR4B+ocr4_value;
        cnt_b=loops_b;
        periodic_task_disable_b();
        
        user_func_b();

    }
    else{
        cnt_b--;
    }
}

ISR(TIMER3_COMPC_vect) {
    if (cnt_c==0)
    {
        OCR3C=OCR3C+ocr3c_value;
        cnt_c=loops_c;
        user_func_c();
        
    }
    else{
        cnt_c--;
    }
}

ISR(TIMER5_COMPA_vect) {
    if (cnt_d==0)
    {

        OCR5A=OCR5A+ocr5_value;
        cnt_d=loops_d;
        
        user_func_d();

    }
    else{
        cnt_d--;
    }
}
#endif


static void init_timer3(){
// Set Timer0 to CTC (Clear Timer on Compare Match) mode
    TCCR3A = 0;

    // Set the prescaler to 1024
    TCCR3B = (1 << CS32) | (1 << CS30);

    // Enable global interrupts
    sei();
}

static void init_timer4(){
// Set Timer0 to CTC (Clear Timer on Compare Match) mode
    TCCR4A = 0;

    // Set the prescaler to 1024
    TCCR4B = (1 << CS32) | (1 << CS30);

    // Enable global interrupts
    sei();
}

static void init_timer5(){
// Set Timer0 to CTC (Clear Timer on Compare Match) mode
    TCCR5A = 0;

    // Set the prescaler to 1024
    TCCR5B = (1 << CS32) | (1 << CS30);

    // Enable global interrupts
    sei();
}


void periodic_task_init_a(void (*user_function_a)(void), uint32_t interval_ms_a) {
    user_func_a = user_function_a;
    init_timer3();

    // Calculate the OCR3A value
    /*
    cnt=(interval_ms_a/1000*(F_CPU/1024))/0xFFFF
    */
    cnt_a = (interval_ms_a/1000*(F_CPU/1024))/0xFFFF;//(interval_ms_a*125)>>19;
    loops_a=cnt_a;
    ocr3_value=(uint32_t)interval_ms_a*(F_CPU/1024)/1000-(uint32_t)cnt_a*0xFFFF;
    
    OCR3A = ocr3_value;

    // Enable Timer3 Compare Match A interrupt
    TIMSK3 |= (1 << OCIE3A);
}


void periodic_task_init_b(void (*user_function_b)(void), uint32_t interval_ms_b) {
    user_func_b = user_function_b;
    init_timer4();
    TCNT4 = 0;

    cnt_b = (interval_ms_b/1000*(F_CPU/1024))/0xFFFF;//(interval_ms_a*125)>>19;
    loops_b=cnt_b;
    ocr4_value=(uint32_t)interval_ms_b*(F_CPU/1024)/1000-(uint32_t)cnt_b*0xFFFF;
    
    uint32_t next_compare = (uint32_t)TCNT4 + ocr4_value;
    OCR4B = (uint16_t)(next_compare & 0xFFFF);

    // Enable Timer3 Compare Match B interrupt
    TIFR4 |= (1 << OCF4B);
    TIMSK4 |= (1 << OCIE4B);
}

void periodic_task_disable_b( ) {

    // TIFR1 |= (1 << OCF1A);
    TIMSK4 &= ~(1 << OCIE4B);

}

void periodic_task_init_c(void (*user_function_c)(void), uint32_t interval_ms_c) {
    user_func_c = user_function_c;
    init_timer3();

    cnt_c = (interval_ms_c/1000*(F_CPU/1024))/0xFFFF;//(interval_ms_a*125)>>19;
    loops_c=cnt_c;
    ocr3c_value=(uint32_t)interval_ms_c*(F_CPU/1024)/1000-(uint32_t)cnt_c*0xFFFF;
    
    OCR3C = ocr3c_value;


    // Enable Timer0 Compare Match C interrupt
    TIMSK3 |= (1 << OCIE3C);
}

void periodic_task_init_d(void (*user_function_d)(void), uint32_t interval_ms_d) {
    user_func_d = user_function_d;
    init_timer5();

    cnt_d = (interval_ms_d/1000*(F_CPU/1024))/0xFFFF;//(interval_ms_a*125)>>19;
    loops_d=cnt_d;
    ocr5_value=(uint32_t)interval_ms_d*(F_CPU/1024)/1000-(uint32_t)cnt_d*0xFFFF;
    
    OCR5A = ocr5_value;


    // Enable Timer0 Compare Match C interrupt
    TIMSK5 |= (1 << OCIE5A);
}


// not done yet... 
float get_exact_interval_a(void) {
    return ((float)(OCR3A + 1) / (F_CPU / 1024)) * 1000.0;
}

