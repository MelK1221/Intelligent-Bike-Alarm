// Bluetooth alert task sends message to bluetooth app if alarm has been triggered

#include "bt_alert.h"

void send_bt_alert(void) {
    static bool already_reported = false; // Track if alert has already been sent

    // If alarm is triggered and message has not yet been sent, send ALERT
    if (is_alarm_triggered() && !already_reported) {
        print_timestamp();
        printf("ALERT: Alarm has been triggered!\n");
        already_reported = true;

    // If alarm disarmed, reset alert sent flag
    } else if (!is_alarm_triggered()) {
        already_reported = false;
    }
}