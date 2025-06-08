#include "controller_state.h"
#include "rtos.h"
#include "sleep.h"

#define SLEEP_TIMEOUT_MS 60000UL  // 60 seconds

void sleep_check(void) {
    static uint32_t last_motion_time = 0;
    static bool in_sleep_mode = false;

    if (is_motion_detected() || is_alarm_triggered()) {
        last_motion_time = rtos_clock_count;

        if (in_sleep_mode) {
            in_sleep_mode = false;
        }

        return;
    }

    // If disarmed, enter sleep immediately
    if (!is_alarm_armed()) {
        if (!in_sleep_mode) {
            enter_sleep_mode();
            in_sleep_mode = true;
        }
        return;
    }

    // If armed and no motion for 60s, go to sleep
    if (!in_sleep_mode &&
        (rtos_clock_count - last_motion_time) >= SLEEP_TIMEOUT_MS) {
        enter_sleep_mode();
        in_sleep_mode = true;
    }
}
