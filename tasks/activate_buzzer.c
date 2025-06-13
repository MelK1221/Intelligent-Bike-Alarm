#include "activate_buzzer.h"

void buzzer_alert(void) {
    static bool buzzer_on = false;
    static uint32_t last_switch_time = 0;

    if (is_alarm_triggered()) {
        if (!buzzer_on && last_switch_time == 0) {
            last_switch_time = rtos_get_clock_count();
        }
        
        if (rtos_get_clock_count() - last_switch_time >= 1000) {
            buzzer_on = !buzzer_on;
            if (buzzer_on) {
                buzz_on();
            } else {
                buzz_off();
            }
            last_switch_time = rtos_get_clock_count();
        }
    } else {
        if (buzzer_on) {
            buzz_off();
            buzzer_on = false;
        }
        last_switch_time = 0;
    }
}