#include "gtest/gtest.h"
extern "C" {
#include "rfid.h"
#include "test_mocks.h"
}

class RfidTest : public ::testing::Test {
protected:
    void SetUp() override { 
        reset_check_rfid_mocks();
        reset_rfid_scan_state();
    }
};

// Helper to set a non-zero tag (present)
void set_present_tag() {
    for (int i = 0; i < UID_LENGTH; ++i) mock_tag.uid[i] = (i==0) ? 0xAA : 0; // At least one non-zero
}

TEST_F(RfidTest, NoScanIfIntervalNotElapsed) {
    mock_clock_count = 500;
    set_present_tag();
    check_rfid();
    EXPECT_EQ(calls_toggle_alarm_armed, 0);
}

TEST_F(RfidTest, NoActionIfNoTagPresent) {
    mock_clock_count = 2000;
    for (int i = 0; i < UID_LENGTH; ++i) mock_tag.uid[i] = 0;
    check_rfid();
    EXPECT_EQ(calls_toggle_alarm_armed, 0);
}

TEST_F(RfidTest, TogglesAlarmIfAuthorizedTag) {
    mock_clock_count = 2000;
    set_present_tag();
    mock_is_authorized_tag_result = true;
    mock_alarm_triggered = false;
    mock_alarm_armed = false;

    check_rfid();

    EXPECT_EQ(calls_toggle_alarm_armed, 1);
    EXPECT_EQ(calls_set_wait_rfid, 1);
    EXPECT_FALSE(set_wait_rfid_last_param);
    EXPECT_EQ(calls_buzz_tone_seq, 1);
    EXPECT_EQ(buzz_tone_seq_last_param, ARMED_BUZZ); // or DISARMED_BUZZ depending on test
}

TEST_F(RfidTest, SetsDisarmedBeepAndAlarmTrigger) {
    mock_clock_count = 2000;
    set_present_tag();
    mock_is_authorized_tag_result = true;
    mock_alarm_triggered = false;
    mock_alarm_armed = true;

    check_rfid();

    // Should call buzz_tone_seq with DISARMED_BUZZ and reset alarm triggered
    EXPECT_EQ(buzz_tone_seq_last_param, DISARMED_BUZZ);
    EXPECT_EQ(calls_set_alarm_triggered, 1);
    EXPECT_FALSE(set_alarm_triggered_last_param);
}

TEST_F(RfidTest, DoesNothingIfUnauthorizedTag) {
    mock_clock_count = 2000;
    set_present_tag();
    mock_is_authorized_tag_result = false;

    check_rfid();

    EXPECT_EQ(calls_toggle_alarm_armed, 0);
    EXPECT_EQ(calls_buzz_tone_seq, 0);
}