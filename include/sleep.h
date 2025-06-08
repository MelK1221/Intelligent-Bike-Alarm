#ifndef SLEEP_DRIVER_H
#define SLEEP_DRIVER_H

extern uint32_t last_motion_time;

void init_sleep(void);
void enter_sleep_mode(void);

#endif