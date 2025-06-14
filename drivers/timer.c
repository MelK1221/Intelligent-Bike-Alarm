// Timer driver initializes clock timer for RTOS and ISR to tick clock

#include "timer.h"

// Initialize 1ms timer for 16Mhz system clock
void init_timer1(void) {
    cli();
    
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    OCR1A = 249;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS11) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);

    sei();
}

// Interrupt service routine ticks RTOS clock every 1 ms
ISR(TIMER1_COMPA_vect) {
    rtos_clock();
}

// Prints amount of time system has been powered on
void print_timestamp(void) {
    uint32_t ms = rtos_get_clock_count();

    uint32_t minutes = ms / 60000;
    uint32_t seconds = (ms % 60000) / 1000;
    uint32_t millis  = ms % 1000;

    printf("[%02lu:%02lu.%03lu] ", minutes, seconds, millis);
}
