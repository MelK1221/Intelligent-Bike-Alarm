#include "controller_state.h"

volatile bool move_detected = false;
volatile bool triggered = false;

static volatile bool armed = false;

bool is_system_armed(void) {
    return armed;
}

void set_system_armed(bool state) {
    armed = state;
}

void toggle_system_armed(void) {
    armed = !armed;
}