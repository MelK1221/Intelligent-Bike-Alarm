#include "gtest/gtest.h"
extern "C" {
#include "sleep_check.h"
#include "test_mocks.h"
}

class SleepCheckTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset all relevant mock variables
        mock_alarm_triggered = false;
        mock_alarm_armed = false;
        mock_wait_rfid = false;
        mock_clock_count = 0;
        wait_rfid_start_time = 0;
        armed_wake_time = 0;
        in_sleep_mode = false;

        buzz_off_calls = 0;
        calls_set_wait_rfid = 0;
        set_wait_rfid_last_param = false;
        enter_sleep_mode_calls = 0;
    }
};

TEST_F(SleepCheckTest, DoesNotSleepIfAlarmTriggered) {
    mock_alarm_triggered = true;
    in_sleep_mode = false;

    check_sleep();

    EXPECT_FALSE(in_sleep_mode);
    EXPECT_EQ(enter_sleep_mode_calls, 0);
}

TEST_F(SleepCheckTest, WakesUpFromSleepIfAlarmNotTriggered) {
    mock_clock_count = 3003;
    in_sleep_mode = true;
    mock_alarm_triggered = false;

    check_sleep();

    EXPECT_FALSE(in_sleep_mode);
    EXPECT_TRUE(set_wait_rfid_last_param);
    EXPECT_EQ(calls_set_wait_rfid, 1);
    EXPECT_EQ(wait_rfid_start_time, mock_clock_count);
}

TEST_F(SleepCheckTest, StaysAwakeIfWaitingForRFIDAndTimeoutNotReached) {
    mock_wait_rfid = true;
    wait_rfid_start_time = 1000;
    mock_clock_count = 1000 + WAIT_RFID_TIMEOUT_MS - 1;

    check_sleep();

    EXPECT_FALSE(in_sleep_mode);
    EXPECT_TRUE(mock_wait_rfid);
    EXPECT_EQ(calls_set_wait_rfid, 0);
}

TEST_F(SleepCheckTest, EndsRFIDWaitIfTimeoutReached) {
    mock_alarm_armed = true; // Stay awake after RFID timeout
    mock_wait_rfid = true;
    wait_rfid_start_time = 1000;
    mock_clock_count = 1000 + WAIT_RFID_TIMEOUT_MS + 1;

    check_sleep();

    EXPECT_FALSE(in_sleep_mode);             
    EXPECT_FALSE(mock_wait_rfid);
    EXPECT_EQ(calls_set_wait_rfid, 1);
    EXPECT_FALSE(set_wait_rfid_last_param);
}

TEST_F(SleepCheckTest, EntersSleepIfDisarmed) {
    mock_alarm_armed = false;
    mock_wait_rfid = false;

    check_sleep();

    EXPECT_TRUE(in_sleep_mode);
    EXPECT_EQ(buzz_off_calls, 1);
    EXPECT_EQ(enter_sleep_mode_calls, 1);
}

TEST_F(SleepCheckTest, EntersSleepOnArmedTimeout) {
    mock_alarm_armed = true;
    armed_wake_time = 1000;
    mock_clock_count = 1000 + SLEEP_TIMEOUT_MS + 1;

    check_sleep();

    EXPECT_TRUE(in_sleep_mode);
    EXPECT_EQ(enter_sleep_mode_calls, 1);
}

TEST_F(SleepCheckTest, StaysAwakeIfArmedAndNoTimeout) {
    mock_alarm_armed = true;
    armed_wake_time = 1000;
    mock_clock_count = 1000 + SLEEP_TIMEOUT_MS - 1;

    check_sleep();

    EXPECT_FALSE(in_sleep_mode);
    EXPECT_EQ(enter_sleep_mode_calls, 0);
}