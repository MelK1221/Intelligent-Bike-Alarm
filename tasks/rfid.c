#include "mfrc522.h"
#include "mfrc522_header.h"
#include "buzzer.h"
#include "controller_state.h"
#include "uart.h"
#include "rtos.h"

#define RFID_CS_PIN  PB2  // D10
#define RFID_RST_PIN PB1  // D9

MFRC522 rfid;

void check_rfid(void) {
    static uint32_t last_scan_time = 0;

    if (rtos_get_clock_count() - last_scan_time < 1000) {
        return;  // debounce
    }

    if (mfrc522_tag_present()) {
        rfid_tag_t tag = mfrc522_get_tag();

        if (is_authorized_tag(&tag)) {
            toggle_system_armed();
            triggered = false;
            move_detected = false;

            if (system_armed()) {
                buzz_tone_seq(BUZZER_ARMED);  // Armed sound
            } else {
                buzz_tone_seq(BUZZER_DISARMED);  // Disarmed sound
            }

            last_scan_time = rtos_get_clock_count();
        }
    }
}
