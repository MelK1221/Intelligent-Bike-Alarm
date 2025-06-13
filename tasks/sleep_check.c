#include "sleep_check.h"

#define SLEEP_TIMEOUT_MS 90000UL  // 90 seconds
#define WAIT_RFID_TIMEOUT_MS 30000UL // 30 seconds

uint32_t wait_rfid_start_time = 0;
volatile bool in_sleep_mode = false;

void check_sleep(void) {

    if (is_motion_detected() || is_alarm_triggered()) {
        last_motion_time = rtos_get_clock_count();
        if (in_sleep_mode) {
            // We just woke up
            DEBUG_PRINT("Woke up due to motion or alarm.\n");
            //last_wakeup_time = rtos_get_clock_count();
            set_wait_rfid(true);
            wait_rfid_start_time = rtos_get_clock_count();
            in_sleep_mode = false; // Now awake
        }

        //set_motion_detected(false);
        return;
    }
    
    // If waiting for RFID, check for timeout
    if (is_wait_rfid()) {
        if ((rtos_get_clock_count() - wait_rfid_start_time) < WAIT_RFID_TIMEOUT_MS) {
            // Still waiting for RFID, stay awake
            return;
        } else {
            // Timeout expired, stop waiting
            set_wait_rfid(false);
            DEBUG_PRINT("RFID wait timeout expired.\n");
        }
    }


    // If disarmed, enter sleep immediately
    if (!is_alarm_armed()) {
        if (is_wait_rfid()) {
            DEBUG_PRINT("Staying awake: waiting for RFID. \n");
            return;
        }
        
        if (!in_sleep_mode) {
            buzz_off();
            in_sleep_mode = true;
            DEBUG_PRINT("Entering sleep: system disarmed.\n");
            rtos_delay_ms(10);
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
