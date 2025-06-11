#include "sleep.h"

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "sleep_check.h"
#include "mfrc522_helper.h"
#include "rtos.h"

volatile uint32_t last_motion_time = 0;
volatile uint32_t last_wakeup_time = 0;

void init_sleep(void) {
    last_motion_time = rtos_get_clock_count();
}

void enter_sleep_mode(void) {
    in_sleep_mode = true;

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    cli(); // Prevent race conditions

    EIFR |= (1 << INTF0);  // Clear any pending INT0 flag
#if defined(sleep_bod_disable)
    sleep_bod_disable(); // Disable BOD if supported
#endif

    sei();

    sleep_cpu(); // MCU sleeps here

    // --- MCU resumes here after wakeup ---
    sleep_disable();

    // Wakeup logic: ensure flags/timestamps are correct
    last_wakeup_time = rtos_get_clock_count();

    // If you need to re-init RFID, do it here:
    // MFRC522_Init(&rfid, RFID_CS_PIN, RFID_RST_PIN);

    DEBUG_PRINT("MCU woke up from sleep.\n");
}