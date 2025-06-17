/* Buzzer driver
* Sets buzzer digital pin (D3) as an output
* Methods for turning buzzer on/off
* Buzzer tone sequence method for arm/disarm
*/

#include "buzzer.h"

#define BUZZER_DDR  DDRD
#define BUZZER_PORT PORTD
#define BUZZER_PIN  PD3

// Initialize buzzer as output on D3 and set to off
void init_buzz(void) {
    BUZZER_DDR |= (1 << BUZZER_PIN);
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
    DEBUG_PRINT("Buzzer Initialized\n");
}

// Turns buzzer on
void buzz_on(void) {
    BUZZER_PORT |= (1 << BUZZER_PIN);
}

// Turns buzzer off
void buzz_off(void) {
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
}

// Tone sequence method to play when arming/disarming alarm
void buzz_tone_seq(buzzer_sequence_t sequence) {

    switch (sequence) {
        // Three beeps to arm
        case ARMED_BUZZ:
            for (int i = 0; i < 3; i++) {
                buzz_on();
                rtos_delay_ms(100);
                buzz_off();
                rtos_delay_ms(100);
            }
            break;

        // One long beep to disarm
        case DISARMED_BUZZ:
            buzz_on();
            rtos_delay_ms(500);
            buzz_off();
            break;
    }
}
