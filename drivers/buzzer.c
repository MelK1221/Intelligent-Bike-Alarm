#include <avr/io.h>
#include "buzzer.h"

#define BUZZER_DDR  DDRD
#define BUZZER_PORT PORTD
#define BUZZER_PIN  PD3  // Adjust if you're using a different pin

void buzz_init(void) {
    BUZZER_DDR |= (1 << BUZZER_PIN);   // Set as output
    BUZZER_PORT &= ~(1 << BUZZER_PIN); // Start OFF
}

void buzz_on(void) {
    BUZZER_PORT |= (1 << BUZZER_PIN);
}

void buzz_off(void) {
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
}
