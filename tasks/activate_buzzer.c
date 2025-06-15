// Activate buzzer task sounds bike alarm if system is in triggered state

#include "activate_buzzer.h"

static bool buzzer_on = false; // Track buzzer status for intermitent beeping
static uint32_t last_switch_time = 0; // Track length of buzz on/off

#ifdef UNIT_TEST
#include <stdio.h>
extern int buzz_on_calls;
extern int buzz_off_calls;
#define LOG_STATE() printf("buzzer_on=%d, last_switch_time=%u, now=%u, buzz_on_calls=%d, buzz_off_calls=%d\n", buzzer_on, last_switch_time, rtos_get_clock_count(), buzz_on_calls, buzz_off_calls)
#else
#define LOG_STATE()
#endif

void buzzer_alert(void) {
    LOG_STATE();
    // Only sound alarm if alarm_triggered is true
    if (is_alarm_triggered()) {
        if (!buzzer_on && last_switch_time == 0) {
            last_switch_time = rtos_get_clock_count();
        }
        
        // Switch buzz on/off every 1 second
        if (rtos_get_clock_count() - last_switch_time >= 1000) {
            buzzer_on = !buzzer_on;
            if (buzzer_on) {
                buzz_on();
            } else {
                buzz_off();
            }
            last_switch_time = rtos_get_clock_count();
        }
    // If alarm is not trigger, make sure buzzer is off and reset local vars
    } else {
        if (buzzer_on) {
            buzz_off();
            buzzer_on = false;
        }
        last_switch_time = 0;
    }
}

#ifdef UNIT_TEST
void reset_buzzer_alert_state(void) {
    // Reset static state variables to initial values
    buzzer_on = false;
    last_switch_time = 0;
}
#endif