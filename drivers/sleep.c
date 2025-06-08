#include "sleep.h"

#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "rtos.h"

uint32_t last_motion_time = 0;

void init_sleep(void) {
    last_motion_time = rtos_clock_count;
}

void enter_sleep_mode(void) {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    cli(); // Prevent race conditions
#if defined(sleep_bod_disable)
    sleep_bod_disable(); // Disable BOD if supported
#endif
    sei();
    sleep_cpu(); // MCU sleeps here
    sleep_disable(); // MCU resumes here
}