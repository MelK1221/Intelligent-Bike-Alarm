#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>

#include "rtos.h"

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

ISR(TIMER1_COMPA_vect) {
    rtos_clock();
}

void print_timestamp(void) {
    uint32_t ms = rtos_get_clock_count();

    uint32_t minutes = ms / 60000;
    uint32_t seconds = (ms % 60000) / 1000;
    uint32_t millis  = ms % 1000;

    printf("[%02lu:%02lu.%03lu] ", minutes, seconds, millis);
}
