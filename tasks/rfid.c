#include "rfid.h"
#include "mfrc522.h"
#include "mfrc522_helper.h"
#include "buzzer.h"
#include "controller_state.h"
#include "uart.h"
#include "rtos.h"
#include "sleep_check.h"
#include <avr/wdt.h>

#define RFID_CS_PIN  PB2  // D10
#define RFID_RST_PIN PB1  // D9
#define RFID_WAKEUP_COOLDOWN_MS 100  // Wait 100ms after wakeup before scanning

extern volatile uint32_t last_wakeup_time;

void check_rfid(void) {
    static uint32_t last_scan_time = 0;
    static bool last_tag_present = false;

    uint32_t now = rtos_get_clock_count();

    //DEBUG_PRINT("check_rfid running, now=%lu\n", now);
    //DEBUG_PRINT("last_scan_time=%lu, elapsed=%lu\n", last_scan_time, now - last_scan_time);
    // DEBUG_PRINT("last_wakeup_time=%lu, since_wakeup=%lu\n", last_wakeup_time, now - last_wakeup_time);
    // DEBUG_PRINT("last_tag_present=%d\n", last_tag_present);

    // 1. Wait a short cooldown after wakeup before scanning RFID
    if ((last_wakeup_time != 0) && (now - last_wakeup_time < RFID_WAKEUP_COOLDOWN_MS)) {
        //DEBUG_PRINT("Return due to wakeup_time");
        return;
    }

    // 2. Debounce: only scan once per 200ms
    if (now - last_scan_time < 200) {
        //DEBUG_PRINT("Return due to last_scan_time");
        return;
    }
    last_scan_time = now;

    //wdt_reset();

    rfid_tag_t tag = mfrc522_get_tag();

    bool tag_present = false;
    for (int i = 0; i < UID_LENGTH; ++i) {
        if (tag.uid[i] != 0) {
            tag_present = true;
            break;
        }
    }

    if (tag_present && is_authorized_tag(&tag)) {
        DEBUG_PRINT("Tag UID: %02X %02X %02X %02X\n",
            tag.uid[0], tag.uid[1], tag.uid[2], tag.uid[3]);
    }

    if (!tag_present) {
         //DEBUG_PRINT("Return due to no tag present\n");
        // No tag, do not process further
        return;
    }

    // Only print 'No tag present' on state change
    if (!tag_present && last_tag_present) {
        //DEBUG_PRINT("No tag present\n");
        last_tag_present = false;
        return;
    }
    //DEBUG_PRINT("Change last tag state to true");
    last_tag_present = true;

    if (is_authorized_tag(&tag)) {
        toggle_alarm_armed();
        set_alarm_triggered(false);
        set_motion_detected(false);

        if (is_alarm_armed()) {
            buzz_tone_seq(ARMED_BUZZ);
        } else {
            buzz_tone_seq(DISARMED_BUZZ);
        }
        //set_wait_rfid(false);

        DEBUG_PRINT("RFID tag scanned: system %s\n", is_alarm_armed() ? "armed" : "disarmed");
    } else {
        DEBUG_PRINT("Unauthorized RFID tag detected.\n");
    }
}
