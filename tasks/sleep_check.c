#include "sleep_check.h"

#include "controller_state.h"
#include "rtos.h"
#include "sleep.h"

#define SLEEP_TIMEOUT_MS 60000UL  // 60 seconds

void check_sleep(void) {
    static bool in_sleep_mode = false;

    if (is_motion_detected() || is_alarm_triggered()) {
        last_motion_time = rtos_get_clock_count();

        if (in_sleep_mode) {
            in_sleep_mode = false;
            DEBUG_PRINT("Woke up due to motion or alarm.\n");
        }

        set_motion_detected(false);

        set_wait_rfid(true);

        return;
    }

    // If disarmed, enter sleep immediately
    if (!is_alarm_armed()) {
        if (is_wait_rfid()) {
            DEBUG_PRINT("Staying awake: waiting for RFID. \n");
            return;
        }
        
        if (!in_sleep_mode) {
            enter_sleep_mode();
            in_sleep_mode = true;
            DEBUG_PRINT("Entering sleep: system disarmed.\n");
        }
        return;
    }

    // If armed and no motion for 60s, go to sleep
    if (!in_sleep_mode &&
        (rtos_get_clock_count() - last_motion_time) >= SLEEP_TIMEOUT_MS) {
        enter_sleep_mode();
        in_sleep_mode = true;
        DEBUG_PRINT("Entering sleep: timeout expired.\n");
    }
}
