#include "rfid.h"

#define RFID_CS_PIN  PB2  // D10
#define RFID_RST_PIN PB1  // D9
#define RFID_WAKEUP_COOLDOWN_MS 100  // Wait 100ms after wakeup before scanning

void check_rfid(void) {
    static uint32_t last_scan_time = 0;
    static bool last_tag_present = false;

    uint32_t now = rtos_get_clock_count();

    // Scan once per second
    if (now - last_scan_time < 1000) {
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
    //DEBUG_PRINT("Tag Present = %d\n", tag_present);

    if (tag_present && is_authorized_tag(&tag)) {
        //DEBUG_PRINT("Tag UID: %02X %02X %02X %02X\n",
        //    tag.uid[0], tag.uid[1], tag.uid[2], tag.uid[3]);
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
        //DEBUG_PRINT("Authorized Tag = %d\n", is_authorized_tag(&tag));
        
        toggle_alarm_armed();

        if (!is_alarm_triggered()) {
            if (is_alarm_armed()) {
                buzz_tone_seq(ARMED_BUZZ);
                armed_wake_time = rtos_get_clock_count();
            } else {
                buzz_tone_seq(DISARMED_BUZZ);
                set_alarm_triggered(false);
            }

            set_wait_rfid(false);
        }

        //DEBUG_PRINT("RFID tag scanned: system %s\n", is_alarm_armed() ? "armed" : "disarmed");
    } else {
        DEBUG_PRINT("Unauthorized RFID tag detected.\n");
    }
}