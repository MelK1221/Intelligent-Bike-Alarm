#include "sleep_check.h"

#define SLEEP_TIMEOUT_MS 90000UL  // 90 seconds
#define WAIT_RFID_TIMEOUT_MS 30000UL // 30 seconds

uint32_t wait_rfid_start_time = 0;
uint32_t armed_wake_time = 0;
volatile bool in_sleep_mode = false;

void check_sleep(void) {

    // If alarm is triggered, do not enter sleep mode
    if (is_alarm_triggered()) {
        if (in_sleep_mode) {
            in_sleep_mode = false;
        }
        
        return;
    }

    // If just woke up and not triggered, start wait time
    if (in_sleep_mode) {
        DEBUG_PRINT("Woke up due to motion or alarm.\n");
        set_wait_rfid(true);
        wait_rfid_start_time = rtos_get_clock_count();
        in_sleep_mode = false;

        return;
    }
    
    // If already awake and waiting for RFID, check timeout
    if (is_wait_rfid()) {
        if ((rtos_get_clock_count() - wait_rfid_start_time) < WAIT_RFID_TIMEOUT_MS) {
            return;
        } else {
            set_wait_rfid(false);
            DEBUG_PRINT("RFID wait timeout expired.\n");
        }
    }

    // If disarmed and not waiting for RFID, enter sleep immediately
    if (!is_alarm_armed()) {
        buzz_off();
        in_sleep_mode = true;
        DEBUG_PRINT("Entering sleep: system disarmed.\n");
        rtos_delay_ms(10);
        enter_sleep_mode();
        return;
    }

    // If armed and no motion for 60s, go to sleep
    if ((rtos_get_clock_count() - armed_wake_time) >= SLEEP_TIMEOUT_MS) {
        in_sleep_mode = true;
        DEBUG_PRINT("Entering sleep: timeout expired.\n");
        enter_sleep_mode();
    }
}
