// Timer driver header file

#ifndef TIMER_H
#define TIMER_H

#ifdef __AVR__
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

#include <stdio.h>
#include <stdint.h>

#include "rtos.h"

void init_timer1(void);
void print_timestamp(void);

#endif // TIMER_H