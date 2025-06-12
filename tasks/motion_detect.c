
// #include "controller_state.h"
// #include "buzzer.h"
// #include "uart.h"
// #include "rtos.h"

// #define RFID_DELAY_AFTER_MOTION_MS 100

// // Module-static variables
// static uint32_t last_motion_time = 0;
// static bool rfid_ready = true; // Expose this via a getter if needed

// void detect_motion(void) {
//     // Check if motion was detected (set by ISR)
//     if (is_motion_detected()) {
//         DEBUG_PRINT("Motion detected!\n");

//         // Only trigger alarm if system is armed and alarm not already triggered
//         if (is_alarm_armed() && !is_alarm_triggered()) {
//             set_alarm_triggered(true);
//             //buzz_tone_seq(ALARM_BUZZ);
//             DEBUG_PRINT("Alarm triggered due to motion!\n");
//         }

//         // Start RFID cooldown period
//         last_motion_time = rtos_get_clock_count();
//         rfid_ready = false;

//         // Clear the motion flag so we only handle once per event.
//         set_motion_detected(false);
//     }

//     // Check if RFID is not ready and cooldown has passed
//     if (!rfid_ready && (rtos_get_clock_count() - last_motion_time >= RFID_DELAY_AFTER_MOTION_MS)) {
//         rfid_ready = true;
//         DEBUG_PRINT("RFID ready after motion cooldown.\n");
//     }
// }

// // Add this getter if your RFID scan logic is in a different file
// bool is_rfid_ready(void) {
//     return rfid_ready;
// }

#include "controller_state.h"
#include "buzzer.h"
#include "uart.h"
#include "rtos.h"


// Example: task runs every ~100ms
void detect_motion(void) {
    // Check if motion was detected (set by ISR)
    if (is_motion_detected()) {
        //DEBUG_PRINT("Motion detected!\n");

        // Only trigger alarm if system is armed and alarm not already triggered
        if (is_alarm_armed() && !is_alarm_triggered()) {
            set_alarm_triggered(true);
            //buzz_tone_seq(ALARM_BUZZ);
            //DEBUG_PRINT("Alarm triggered due to motion!\n");
        }

        // Clear the motion flag so we only handle once per event.
        //set_motion_detected(false);
    }
}