#include <gtest/gtest.h>

extern "C" {
#include "bt_alert.h"
}

// --- Mocked globals & functions ---
static bool mock_alarm_triggered = false;
static int print_timestamp_calls = 0;
static std::string captured_output;

// Mocks
extern "C" {
bool is_alarm_triggered(void) { return mock_alarm_triggered; }
void print_timestamp(void) { print_timestamp_calls++; }
int printf(const char *fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    captured_output += buf;
    return strlen(buf);
}
void reset_bt_alert_state(void);
}

TEST(BTAlertTest, SendsAlertStringOnTrigger) {
    reset_bt_alert_state();
    captured_output.clear();
    print_timestamp_calls = 0;

    // Should print alert string when triggered
    mock_alarm_triggered = true;
    send_bt_alert();
    ASSERT_GT(captured_output.find("ALERT: Alarm has been triggered!"), std::string::npos);
    EXPECT_EQ(print_timestamp_calls, 1);

    // Should not print it again while still triggered
    captured_output.clear();
    send_bt_alert();
    EXPECT_EQ(captured_output, "");

    // Disarm and re-trigger: should print again
    mock_alarm_triggered = false;
    send_bt_alert();
    mock_alarm_triggered = true;
    captured_output.clear();
    send_bt_alert();
    ASSERT_GT(captured_output.find("ALERT: Alarm has been triggered!"), std::string::npos);
}