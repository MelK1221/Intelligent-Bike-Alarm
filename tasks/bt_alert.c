#include <stdio.h>
#include "controller_state.h"
#include "rtos.h"

void send_bt_alert(void) {
    static bool already_reported = false;

    if (is_alarm_triggered() && !already_reported) {
        print_timestamp();
        printf("ALERT: Alarm has been triggered!\n");
        already_reported = true;
    } else if (!is_alarm_triggered()) {
        already_reported = false;
    }
}