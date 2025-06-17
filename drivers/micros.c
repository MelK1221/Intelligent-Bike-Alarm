// Microsecond timer setup exclusively for execution timing tests

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

// Global us counter
volatile uint32_t microseconds = 0;

// Timer0 ISR initialization
ISR(TIMER0_OVF_vect) {
    microseconds += 256 * 4;
}

void micros_init() {
    // Set Timer0 to normal mode
    TCCR0A = 0x00;
    // Set prescaler to 64
    TCCR0B = (1<<CS01) | (1<<CS00);
    // Enable overflow interrupt
    TIMSK0 |= (1<<TOIE0);
    // Enable global interrupts
    sei();
}

uint32_t micros_custom() {
    uint32_t us;
    uint8_t tcnt;
    uint8_t oldSREG = SREG;
    cli(); // Disable interrupts to avoid race condition
    tcnt = TCNT0;
    us = microseconds;
    // If an overflow happened but ISR hasn't run yet
    if ((TIFR0 & (1<<TOV0)) && (tcnt < 255)) {
        us += 256 * 4;
    }
    SREG = oldSREG; // Restore interrupts
    us += tcnt * 4;
    return us;
}