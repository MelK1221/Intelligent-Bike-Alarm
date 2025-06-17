// Real-time operating system header file

#ifndef RTOS_H
#define RTOS_H

#ifdef __AVR__
#include <avr/interrupt.h>
#include <avr/wdt.h>
#endif

#include <stdint.h>
#include <stdio.h>

#include "micros.h"

// Debugging printing Macro
#ifdef DEBUG_RTOS
#define DEBUG_PRINT(...) do { printf(__VA_ARGS__); fflush(stdout); } while (0)
#else
#define DEBUG_PRINT(...)
#endif

// Maximum of 8 tasks can be scheuduled
#define MAX_TASKS 8

typedef void (*Task)(void);

// Each task scheduled with period and offset start time
typedef struct {
    Task task;
    uint16_t period;
    uint16_t offset;
} AddedTask;

void rtos_init(void);
void rtos_add_task(Task task_call, uint16_t period, uint16_t offset);
void rtos_clock(void);
uint32_t rtos_get_clock_count(void);
void rtos_scheduler(void);
void rtos_delay_ms(uint32_t delay_ms);

#endif // RTOS_H