// Sleep driver initializes sleep wait vars and puts Arduino in sleep mode

#include "sleep.h"

void init_sleep(void) { 
    set_wait_rfid(true);
    wait_rfid_start_time = rtos_get_clock_count();
}

// Enter Arduino sleep mode, exit after motion interrupt
void enter_sleep_mode(void) {
    in_sleep_mode = true;
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    // Disable interrupts
    cli();

    EIFR |= (1 << INTF0);
#if defined(sleep_bod_disable)
    sleep_bod_disable();
#endif

    // Enable interrupts
    sei();

    // Put Arduino in sleep mode
    sleep_cpu();

    // Disable sleep mode after motion interrupt
    sleep_disable();
}