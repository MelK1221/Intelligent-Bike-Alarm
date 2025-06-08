#ifndef CONTROLLER_STATE_H
#define CONTROLLER_STATE_H

#include <stdbool.h>

extern volatile bool move_detected;
extern volatile bool triggered;

bool is_system_armed(void);
void set_system_armed(bool state);
void toggle_system_armed(void);

#endif