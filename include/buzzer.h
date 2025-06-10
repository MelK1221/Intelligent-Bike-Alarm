#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

typedef enum {
    ARMED_BUZZ,
    DISARMED_BUZZ
} buzzer_sequence_t;

void init_buzz(void);
void buzz_on(void);
void buzz_off(void);
void buzz_tone_seq(buzzer_sequence_t sequence);

#endif // BUZZER_H
