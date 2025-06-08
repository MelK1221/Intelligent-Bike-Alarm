#include <avr/io.h>
#include <avr/interrupt.h>

void timer1_init_10ms(void) {
    TCCR1A = 0;        // Normal port operation
    TCCR1B = 0;        // Clear settings
    TCNT1 = 0;         // Reset counter

    OCR1A = 2499;      // Compare match value for 10ms at 16MHz / 64
    TCCR1B |= (1 << WGM12);                // CTC mode (WGM12=1)
    TCCR1B |= (1 << CS11) | (1 << CS10);   // Prescaler = 64
    TIMSK1 |= (1 << OCIE1A);               // Enable Timer1 Compare A Match interrupt
}

ISR(TIMER1_COMPA_vect) {
    static uint16_t count = 0;
    if (++count >= 50) {  // 50 × 10ms = 500ms
        PORTB ^= (1 << PB5);  // Toggle LED
        count = 0;
    }
}

int main(void) {
    DDRB |= (1 << PB5);   // Set PB5 (D13 LED) as output
    timer1_init_10ms();   // Setup timer
    sei();                // Enable global interrupts

    while (1) {
        // Do nothing, just wait for ISR
    }
}
