#ifndef SLEEP_H
#define SLEEP_H

#include <stdint.h>

extern volatile uint32_t last_motion_time;

void init_sleep(void);
void enter_sleep_mode(void);

#endif // SLEEP_H