#ifndef RTOS_H
#define RTOS_H

#include <stdint.h>
#include <stdio.h>

// For debugging
#ifdef DEBUG_RTOS
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#define MAX_TASKS 8

typedef void (*Task)(void);

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

//extern volatile uint32_t clock_count;
//extern uint8_t task_count;

#endif