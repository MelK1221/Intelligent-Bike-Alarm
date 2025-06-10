#include "rfid.h"

#include "mfrc522.h"
#include "mfrc522_helper.h"
#include "buzzer.h"
#include "controller_state.h"
#include "uart.h"
#include "rtos.h"

#define RFID_CS_PIN  PB2  // D10
#define RFID_RST_PIN PB1  // D9

void check_rfid(void) {
    static uint32_t last_scan_time = 0;

    // Debounce: only scan once per second
    if (rtos_get_clock_count() - last_scan_time < 1000) {
        return;
    }

    last_scan_time = rtos_get_clock_count();

    rfid_tag_t tag = mfrc522_get_tag();  // Only checks/polls card once

    // Optional: if you add a .valid field to rfid_tag_t, check here
    // if (!tag.valid) return;

    // Old style: skip if UID is all 0s (still needed if no .valid)
    bool is_zero = true;
    for (int i = 0; i < UID_LENGTH; ++i) {
        if (tag.uid[i] != 0) {
            is_zero = false;
            break;
        }
    }
    if (is_zero) {
        return;  // No card or read failed
    }

    if (is_authorized_tag(&tag)) {
        toggle_alarm_armed();
        set_alarm_triggered(false);
        set_motion_detected(false);

        if (is_alarm_armed()) {
            buzz_tone_seq(ARMED_BUZZ);  // Armed sound
        } else {
            buzz_tone_seq(DISARMED_BUZZ);  // Disarmed sound
        }

        set_wait_rfid(false);

        DEBUG_PRINT("RFID tag scanned: system %s\n", is_alarm_armed() ? "armed" : "disarmed");
    } else {
        DEBUG_PRINT("Unauthorized RFID tag detected.\n");
    }
}

// void check_rfid(void) {
//     static uint32_t last_scan_time = 0;

//     if (rtos_get_clock_count() - last_scan_time < 1000) {
//         return;  // debounce
//     }

//     if (MFRC522_PICC_IsNewCardPresent(&rfid)) {
//         last_scan_time = rtos_get_clock_count();
//         rfid_tag_t tag = mfrc522_get_tag();

//         if (is_authorized_tag(&tag)) {
//             toggle_alarm_armed();
//             set_alarm_triggered(false);
//             set_motion_detected(false);

//             if (is_alarm_armed()) {
//                 buzz_tone_seq(ARMED_BUZZ);  // Armed sound
//             } else {
//                 buzz_tone_seq(DISARMED_BUZZ);  // Disarmed sound
//             }

//             set_wait_rfid(false);

//             DEBUG_PRINT("RFID tag scanned: system %s\n", is_alarm_armed() ? "armed" : "disarmed");
//         } else {
//             DEBUG_PRINT("Unauthorized RFID tag detected.\n");
//         }

//     }
// }
