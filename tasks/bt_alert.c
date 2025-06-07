#include "controller_state.h"
#include <stdio.h>
#include "rtos.h"

void send_bt_alert(void) {
    static bool already_reported = false;

    if (move_detected && !already_reported) {
        printf("ALERT: Motion Detected!\n");
        already_reported = true;
    } else if (!move_detected) {
        already_reported = false;
    }
}