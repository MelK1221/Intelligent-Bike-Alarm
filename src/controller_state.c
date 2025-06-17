// Accessor methods for the system state flags

#include "controller_state.h"

// State flags declared as local, only accessible via get/set methods
static volatile bool alarm_armed = false;
static volatile bool alarm_triggered = false;
static volatile bool motion_detected = false;
static volatile bool waiting_for_rfid = false;

// Set state of alarm as armed (true) or disarmed (false)
void set_alarm_armed(bool state) {
    alarm_armed = state;

    if (!state) {
        alarm_triggered = false;
        motion_detected = false;
    }
}

// Toggle state of alarm armed flag
void toggle_alarm_armed(void) {
    set_alarm_armed(!alarm_armed);
}

// Get alarm armed/disarmed state
bool is_alarm_armed(void) {
    return alarm_armed;
}

// Set alarm triggered state to true or false
void set_alarm_triggered(bool state) {
    alarm_triggered = state;
}

// Check current state of alarm triggered flag
bool is_alarm_triggered(void) {
    return alarm_triggered;
}

// Set motion detected state to true or false
void set_motion_detected(bool state) {
    motion_detected = state;
}

// Check state of motion detected flag
bool is_motion_detected(void) {
    return motion_detected;
}

// Set status of waiting for RFID scan to true or false
void set_wait_rfid(bool state) {
    waiting_for_rfid = state;
}

// Check state of waiting for RFID scan flag
bool is_wait_rfid(void) {
    return waiting_for_rfid;
}