// Sleep check task header file

#ifndef SLEEP_CHECK_H
#define SLEEP_CHECK_H

#include <stdbool.h>
#include <stdint.h>

#include "buzzer.h"
#include "sleep.h"
#include "controller_state.h"
#include "rtos.h"

// Timeouts to check before entering sleep mode
#define SLEEP_TIMEOUT_MS 90000UL  // 90 seconds
#define WAIT_RFID_TIMEOUT_MS 30000UL // 30 seconds

extern uint32_t wait_rfid_start_time;
extern uint32_t armed_wake_time;
extern volatile bool in_sleep_mode;

void check_sleep(void);

#endif // SLEEP_CHECK_H