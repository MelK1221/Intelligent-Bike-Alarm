#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern volatile uint32_t rtos_clock_count;

void init_timer1(void);
void print_timestamp(void);

#endif // TIMER_H