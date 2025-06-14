// Sleep check task header file

#ifndef SLEEP_CHECK_H
#define SLEEP_CHECK_H

#include <stdbool.h>
#include <stdint.h>

#include "buzzer.h"
#include "sleep.h"
#include "controller_state.h"
#include "rtos.h"

extern uint32_t wait_rfid_start_time;
extern uint32_t armed_wake_time;
extern volatile bool in_sleep_mode;

void check_sleep(void);

#endif // SLEEP_CHECK_H