#include "buzzer.h"

int buzz_on_calls = 0;
int buzz_off_calls = 0;

void buzz_on(void) { buzz_on_calls++; }
void buzz_off(void) { buzz_off_calls++; }