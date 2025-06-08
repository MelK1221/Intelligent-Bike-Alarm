#ifndef CONTROLLER_STATE_H
#define CONTROLLER_STATE_H

#include <stdbool.h>

extern volatile bool motion_detected;
extern volatile bool alarm_armed;
extern volatile bool alarm_triggered;

bool is_system_armed(void);
void set_system_armed(bool state);
void toggle_system_armed(void);

#endif