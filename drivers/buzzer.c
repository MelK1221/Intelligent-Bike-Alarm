#include "buzzer.h"

#include <avr/io.h>

#define BUZZER_DDR  DDRD
#define BUZZER_PORT PORTD
#define BUZZER_PIN  PD3

void buzz_init(void) {
    BUZZER_DDR |= (1 << BUZZER_PIN);
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
}

void buzz_on(void) {
    BUZZER_PORT |= (1 << BUZZER_PIN);
}

void buzz_off(void) {
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
}

void buzz_tone_seq(buzzer_sequence_t sequence) {
    BUZZER_DDR |= (1 << BUZZER_PIN);  // Set buzzer pin as output

    switch (sequence) {
        case BUZZER_ARMED:
            // Example: 3 short beeps
            for (int i = 0; i < 3; i++) {
                BUZZER_PORT |= (1 << BUZZER_PIN);   // buzzer ON
                _delay_ms(100);
                BUZZER_PORT &= ~(1 << BUZZER_PIN);  // buzzer OFF
                _delay_ms(100);
            }
            break;

        case BUZZER_DISARMED:
            // Example: 1 long beep
            BUZZER_PORT |= (1 << BUZZER_PIN);       // buzzer ON
            _delay_ms(500);
            BUZZER_PORT &= ~(1 << BUZZER_PIN);      // buzzer OFF
            break;
    }
}
