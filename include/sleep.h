#ifndef SLEEP_H
#define SLEEP_H

#include <stdint.h>
#include "rtos.h"

extern volatile uint32_t last_motion_time;
extern volatile uint32_t last_wakeup_time;

void init_sleep(void);
void enter_sleep_mode(void);

#endif // SLEEP_H