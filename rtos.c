#include "rtos.h"
#include <stdio.h>

static AddedTask task_list[MAX_TASKS];
uint8_t task_count = 0;
volatile uint32_t rtos_clock_count = 0;

void rtos_init(void) {
    task_count = 0;
    rtos_clock_count = 0;
}

void rtos_add_task(Task task_call, uint16_t period, uint16_t offset) {
    if (task_count < MAX_TASKS) {
        task_list[task_count].task = task_call;
        task_list[task_count].period = period;
        task_list[task_count].offset = offset;
        printf("Task added: %u (period=%u offset=%u)\r\n", task_count, period, offset);  // Debug
        task_count++;
    } else {
        printf("Task list full!\r\n");
    }
}
void rtos_clock(void) {
    rtos_clock_count++;
    //printf("Clock count: %lu\r\n", rtos_clock_count);
}

void rtos_scheduler(void) {
    static uint32_t last_clock_count = 0;

    if (rtos_clock_count != last_clock_count) {
        last_clock_count = rtos_clock_count;

        // printf("Scheduler tick: %lu\r\n", rtos_clock_count);  // Debug here

        for (uint8_t i = 0; i < task_count; i++) {
            if ((rtos_clock_count - task_list[i].offset) % task_list[i].period == 0) {
                // printf("Running task %d\r\n", i);  // Add this too
                task_list[i].task();
            }
        }
    }
}

// Waits for ms milliseconds using rtos_clock_count
void delay_ms_rtos(uint32_t ms) {
    uint32_t start = rtos_clock_count;
    while ((rtos_clock_count - start) < ms) {
       // rtos_scheduler();  // Let other RTOS tasks run during delay
    }
}



