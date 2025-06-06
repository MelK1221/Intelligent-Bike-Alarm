#ifndef RTOS_H
#define RTOS_H

#include <stdint.h>

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
void rtos_scheduler(void);
void delay_ms_rtos(uint32_t ms);

extern volatile uint32_t rtos_clock_count;
extern uint8_t task_count;

#endif