#include "controller_state.h"
#include <stdbool.h>
#include "rtos.h"

static volatile bool alarm_armed = false;
static volatile bool alarm_triggered = false;
static volatile bool motion_detected = false;
static volatile bool waiting_for_rfid = false;

void set_alarm_armed(bool state) {
    alarm_armed = state;

    if (!state) {
        alarm_triggered = false;
        motion_detected = false;
    }
}

void toggle_alarm_armed(void) {
    set_alarm_armed(!alarm_armed);
}

bool is_alarm_armed(void) {
    return alarm_armed;
}

void set_alarm_triggered(bool state) {
    alarm_triggered = state;
}

bool is_alarm_triggered(void) {
    return alarm_triggered;
}

void set_motion_detected(bool state) {
    motion_detected = state;
}

bool is_motion_detected(void) {
    return motion_detected;
}

void set_wait_rfid(bool state) {
    waiting_for_rfid = state;
    //DEBUG_PRINT("waiting_for_rfid = %s\n", waiting_for_rfid ? "true" : "false");

}

bool is_wait_rfid(void) {
    return waiting_for_rfid;
}