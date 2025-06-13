#ifndef MOTION_DETECT_H
#define MOTION_DETECT_H

#include <stdbool.h>
//#include <stdint.h>

#include "controller_state.h"
//#include "buzzer.h"
//#include "uart.h"
#include "rtos.h"

void detect_motion(void);
//bool is_rfid_ready(void);

#endif // MOTION_DETECT_H