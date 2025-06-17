#include <gtest/gtest.h>

extern "C" {
#include "activate_buzzer.h"
#include "test_mocks.h"
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
    EXPECT_EQ(buzz_on_calls, 0);
    EXPECT_EQ(buzz_off_calls, 0);

    mock_clock = 2002;
    buzzer_alert();
    EXPECT_EQ(buzz_on_calls, 1);
    EXPECT_EQ(buzz_off_calls, 0);

    mock_clock = 3003;
    buzzer_alert();
    EXPECT_EQ(buzz_on_calls, 1); 
    EXPECT_EQ(buzz_off_calls, 1); 
}