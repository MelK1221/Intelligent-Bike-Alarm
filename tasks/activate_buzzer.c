#include "buzzer.h"
#include "controller_state.h"
#include "rtos.h"

void buzzer_alert(void) {
    static uint32_t last_buzz_time = 0;
    uint32_t current_time = rtos_get_clock_count();
    if (move_detected && (rtos_get_clock_count() - last_buzz_time >= 1000)) {
            buzz_on();
            rtos_delay_ms(200);  // Short beep
            buzz_off();
            last_buzz_time = current_time
    }
}
