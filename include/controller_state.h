#ifndef CONTROLLER_STATE_H
#define CONTROLLER_STATE_H

#include <stdbool.h>

void set_alarm_armed(bool state);
void toggle_alarm_armed(void);
bool is_alarm_armed(void);
void set_alarm_triggered(bool state);
bool is_alarm_triggered(void);
void set_motion_detected(bool state);
bool is_motion_detected(void);
void set_wait_rfid(bool state);
bool is_wait_rfid(void);

#endif // CONTROLLER_STATE_H