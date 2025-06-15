// Motion detect task controls state of alarm triggered flag

#include "motion_detect.h"

void detect_motion(void) {
    // Check for motion interrupt from ISR
    if (is_motion_detected()) {
        //last_motion_time = rtos_get_clock_count();


        // Only trigger alarm if system is armed and alarm not already triggered
        if (is_alarm_armed() && !is_alarm_triggered()) {
            set_alarm_triggered(true);
            //DEBUG_PRINT("Alarm triggered due to motion!\n");
        }

        // Clear the motion flag so we only handle once per event.
        set_motion_detected(false);
    }
}
