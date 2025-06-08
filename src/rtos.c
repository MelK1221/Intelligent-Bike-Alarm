#include "rtos.h"
#include <stdio.h>

static AddedTask task_list[MAX_TASKS];
static uint8_t task_count = 0;
static volatile uint32_t clock_count = 0;

void rtos_init(void) {
    task_count = 0;
    clock_count = 0;
}

void rtos_clock(void) {
    clock_count++;
}

uint32_t rtos_get_clock_count(void) {
    return clock_count;
}

void rtos_delay_ms(uint32_t delay_ms) {
    uint32_t start = clock_count;
    while ((clock_count - start) < delay_ms) {
       // rtos_scheduler();  // Let other RTOS tasks run during delay
    }
}

void rtos_add_task(Task task_call, uint16_t period, uint16_t offset) {
    if (task_count < MAX_TASKS) {
        task_list[task_count].task = task_call;
        task_list[task_count].period = period;
        task_list[task_count].offset = offset;
        DEBUG_PRINT("Task added: %u (period=%u offset=%u)\r\n", task_count, period, offset);
        task_count++;
    } else {
        DEBUG_PRINT("Task list full!\r\n");
    }
}

void rtos_scheduler(void) {
    static uint32_t last_clock_count = 0;
    static uint32_t start_time = 0;
    static uint32_t end_time = 0;

    if (clock_count != last_clock_count) {
        last_clock_count = clock_count;

        for (uint8_t i = 0; i < task_count; i++) {
            if ((clock_count - task_list[i].offset) % task_list[i].period == 0) {
                start_time = clock_count;
                task_list[i].task();
                end_time = clock_count;

                if ((end_time - start_time) > task_list[i].period) {
                    DEBUG_PRINT("Task %u exceeded period: took %lu ms (period = %u)\r\n", i, (end_time - start_time), task_list[i].period);
                }
            }
        }
    }
}