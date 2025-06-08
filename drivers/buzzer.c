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
