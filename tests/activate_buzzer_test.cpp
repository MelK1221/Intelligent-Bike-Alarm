#include <gtest/gtest.h>

extern "C" {
#include "activate_buzzer.h"
}

// --- Mocked globals & functions ---
extern int buzz_on_calls;
extern int buzz_off_calls;

static bool mock_alarm_triggered = false;
static uint32_t mock_clock = 0;

extern "C" {

// Mock implementation for is_alarm_triggered
bool is_alarm_triggered(void) {
    return mock_alarm_triggered;
}

// Mock implementation for rtos_get_clock_count
uint32_t rtos_get_clock_count(void) {
    return mock_clock;
}

void reset_buzzer_alert_state(void);

}

// --- GoogleTest test cases ---

TEST(ActivateBuzzerTest, AlarmNotTriggeredTurnsBuzzerOffAndResets) {
    reset_buzzer_alert_state();
    // Ensure buzzer is off and time is reset when alarm not triggered
    mock_alarm_triggered = false;
    mock_clock = 0;
    buzz_on_calls = 0;
    buzz_off_calls = 0;

    buzzer_alert();
    EXPECT_EQ(buzz_on_calls, 0); // Should not call buzz_on
    EXPECT_EQ(buzz_off_calls, 0); // Should not call buzz_off (already off)

    // Simulate alarm previously triggered (would require more advanced state management if static)
    // For now, just call again to ensure repeated calls do not call buzz_on/off unnecessarily
    buzzer_alert();
    EXPECT_EQ(buzz_off_calls, 0);
}

TEST(ActivateBuzzerTest, AlarmTriggeredBeepsEverySecond) {
    reset_buzzer_alert_state();
    mock_alarm_triggered = true;
    buzz_on_calls = 0;
    buzz_off_calls = 0;
    mock_clock = 0;

    buzzer_alert();
    EXPECT_EQ(buzz_on_calls, 0);
    EXPECT_EQ(buzz_off_calls, 0);

    mock_clock = 1001;
    buzzer_alert();
    EXPECT_EQ(buzz_on_calls, 0); // No beep yet, just initializes timer
    EXPECT_EQ(buzz_off_calls, 0);

    mock_clock = 2002;
    buzzer_alert();
    EXPECT_EQ(buzz_on_calls, 1); // First beep (on)
    EXPECT_EQ(buzz_off_calls, 0);

    mock_clock = 3003;
    buzzer_alert();
    EXPECT_EQ(buzz_on_calls, 1); // Still only 1 ON
    EXPECT_EQ(buzz_off_calls, 1); // First beep (off)
}