#ifndef MOTION_DETECT_H
#define MOTION_DETECT_H

//#include <stdbool.h>
#include <stdint.h>

#include "sleep_check.h"
#include "controller_state.h"
#include "rtos.h"

extern volatile uint32_t last_motion_time;

void detect_motion(void);

#endif // MOTION_DETECT_H