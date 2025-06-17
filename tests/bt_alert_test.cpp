#include <gtest/gtest.h>

extern "C" {
#include "bt_alert.h"
#include "test_mocks.h"
}

TEST(BTAlertTest, SendsAlertStringOnTrigger) {
    reset_bt_alert_state();
    captured_printf_output[0] = '\0';
    print_timestamp_calls = 0;

    // Should print alert string when triggered
    mock_alarm_triggered = true;
    send_bt_alert();
    ASSERT_NE(strstr(captured_printf_output, "ALERT: Alarm has been triggered!"), nullptr);
    EXPECT_EQ(print_timestamp_calls, 1);

    // Should not print it again while still triggered
    captured_printf_output[0] = '\0';
    send_bt_alert();
    EXPECT_STREQ(captured_printf_output, "");

    // Disarm and re-trigger: should print again
    mock_alarm_triggered = false;
    send_bt_alert();
    mock_alarm_triggered = true;
    captured_printf_output[0] = '\0';
    send_bt_alert();
    ASSERT_NE(strstr(captured_printf_output, "ALERT: Alarm has been triggered!"), nullptr);
}