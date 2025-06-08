#include "buzzer.h"
#include "controller_state.h"
#include "rtos.h"

void buzzer_alert(void) {
    static uint32_t last_buzz_time = 0;
    if (move_detected) {
        if ((rtos_clock_count - last_buzz_time) > 1000) { // Every 1 sec
            buzz_on();
            delay_ms_rtos(200);  // Short beep
            buzz_off();
            last_buzz_time = rtos_clock_count;
        }
    }
}
