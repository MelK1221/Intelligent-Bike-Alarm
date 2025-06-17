// RFID check task scans for rfid tag and toggles alarm armed state if authorized tag scanned

#include "rfid.h"

#define RFID_CS_PIN  PB2
#define RFID_RST_PIN PB1

// For unit testing
#ifdef UNIT_TEST
uint32_t last_scan_time = 0;
#else
static uint32_t last_scan_time = 0;
#endif

void check_rfid(void) {
    //static uint32_t last_scan_time = 0;
    uint32_t now = rtos_get_clock_count();

    // Scan once per second
    if (now - last_scan_time < 1000) {
        return;
    }
    // Save scan time and get tag UID
    last_scan_time = now;
    rfid_tag_t tag = mfrc522_get_tag();

    // Check for presense of tag
    bool tag_present = false;
    for (int i = 0; i < UID_LENGTH; ++i) {
        if (tag.uid[i] != 0) {
            tag_present = true;
            break;
        }
    }

    // No tag present, exit task
    if (!tag_present) {
        return;
    }

    // If authorized tag presented, change alarm state
    if (is_authorized_tag(&tag)) {
        
        // Switch alarm armed flag state if authorized tag presented
        toggle_alarm_armed();

        // If alarm not triggering, play arm/disarm beep seq for user notification
        if (!is_alarm_triggered()) {
            if (is_alarm_armed()) {
                buzz_tone_seq(ARMED_BUZZ);
                armed_wake_time = rtos_get_clock_count();
            } else {
                buzz_tone_seq(DISARMED_BUZZ);
            }

            // RFID has just been scanned, no longer waiting or triggering
            set_wait_rfid(false);
            set_alarm_triggered(false);
        }

    } else {
        DEBUG_PRINT("Unauthorized RFID tag detected.\n");
    }
}