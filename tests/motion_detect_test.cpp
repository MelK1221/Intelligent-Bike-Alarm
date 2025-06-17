#include "gtest/gtest.h"
extern "C" {
#include "motion_detect.h"
#include "test_mocks.h"
}

// Define a test fixture class
class DetectMotionTest : public ::testing::Test {
protected:
    void SetUp() override {
        reset_detect_motion_mocks();
    }
};

// Use TEST_F to use the fixture for each test
TEST_F(DetectMotionTest, TriggersAlarmWhenMotionAndArmed) {
    mock_motion_detected = true;
    mock_alarm_armed = true;
    mock_alarm_triggered = false;

    detect_motion();

    EXPECT_EQ(calls_set_alarm_triggered, 1);
    EXPECT_TRUE(set_alarm_triggered_last_param);
    EXPECT_EQ(motion_detected_set_calls, 1);
    EXPECT_FALSE(motion_detected_last_value);
}

TEST_F(DetectMotionTest, DoesNotTriggerAlarmIfNotArmed) {
    mock_motion_detected = true;
    mock_alarm_armed = false; // not armed
    mock_alarm_triggered = false;

    detect_motion();

    EXPECT_EQ(calls_set_alarm_triggered, 0);
    EXPECT_EQ(motion_detected_set_calls, 1);
    EXPECT_FALSE(motion_detected_last_value);
}

TEST_F(DetectMotionTest, DoesNotTriggerAlarmIfAlreadyTriggered) {
    mock_motion_detected = true;
    mock_alarm_armed = true;
    mock_alarm_triggered = true; // already triggered

    detect_motion();

    EXPECT_EQ(calls_set_alarm_triggered, 0);
    EXPECT_EQ(motion_detected_set_calls, 1);
    EXPECT_FALSE(motion_detected_last_value);
}

TEST_F(DetectMotionTest, DoesNothingIfNoMotion) {
    mock_motion_detected = false; // no motion

    detect_motion();

    EXPECT_EQ(calls_set_alarm_triggered, 0);
    EXPECT_EQ(motion_detected_set_calls, 0);
}