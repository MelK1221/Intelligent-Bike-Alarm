#include "sleep_check.h"
#include "controller_state.h"
#include "rtos.h"
#include "sleep.h"

#define SLEEP_TIMEOUT_MS 60000UL  // 60 seconds

volatile bool in_sleep_mode = false;

void check_sleep(void) {

    if (is_motion_detected() || is_alarm_triggered()) {
        last_motion_time = rtos_get_clock_count();
        if (in_sleep_mode) {
            // We just woke up
            DEBUG_PRINT("Woke up due to motion or alarm.\n");
            last_wakeup_time = rtos_get_clock_count();
            set_wait_rfid(true);
            in_sleep_mode = false; // Now awake
        }

        set_motion_detected(false);
        return;
    }

    // If disarmed, enter sleep immediately
    if (!is_alarm_armed()) {
        if (is_wait_rfid()) {
            //DEBUG_PRINT("Staying awake: waiting for RFID. \n");
            return;
        }
        
        if (!in_sleep_mode) {
            in_sleep_mode = true;
            DEBUG_PRINT("Entering sleep: system disarmed.\n");
            enter_sleep_mode();
        }
        return;
    }

    // If armed and no motion for 60s, go to sleep
    if (!in_sleep_mode &&
        (rtos_get_clock_count() - last_motion_time) >= SLEEP_TIMEOUT_MS) {
        in_sleep_mode = true;
        DEBUG_PRINT("Entering sleep: timeout expired.\n");
        enter_sleep_mode();
    }
}
