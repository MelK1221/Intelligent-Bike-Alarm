#include "test_mocks.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// ---- Mocked variables ----
bool mock_alarm_triggered = false;
uint32_t mock_clock = 0;
int buzz_on_calls = 0;
int buzz_off_calls = 0;
int print_timestamp_calls = 0;
char captured_printf_output[1024] = {0};
bool mock_motion_detected = false;
bool mock_alarm_armed = false;
int alarm_triggered_set_calls = 0;
bool alarm_triggered_last_value = false;
int motion_detected_set_calls = 0;
bool motion_detected_last_value = false;

// ---- RFID/check_rfid mocks ----
rfid_tag_t mock_tag;
bool mock_is_authorized_tag_result = false;
int calls_toggle_alarm_armed = 0;
int calls_buzz_tone_seq = 0;
buzzer_sequence_t buzz_tone_seq_last_param = 0;
int calls_set_alarm_triggered = 0;
bool set_alarm_triggered_last_param = false;
int calls_set_wait_rfid = 0;
bool set_wait_rfid_last_param = false;
uint32_t mock_clock_count = 0;
uint32_t last_scan_time;

// ---- Sleep check mocks and globals ----
//uint32_t wait_rfid_start_time = 0;
//uint32_t armed_wake_time = 0;
//volatile bool in_sleep_mode = false;
int rtos_delay_ms_calls = 0;
uint32_t rtos_delay_ms_last = 0;
int enter_sleep_mode_calls = 0;
bool mock_wait_rfid = false;
int set_wait_rfid_calls = 0;
bool set_wait_rfid_last = false;

// ---- Mocked functions ----
bool is_alarm_triggered(void) {
    return mock_alarm_triggered;
}

uint32_t rtos_get_clock_count(void) {
    // For legacy code use mock_clock, for RFID code use mock_clock_count
    return mock_clock_count ? mock_clock_count : mock_clock;
}

void buzz_on(void) {
    buzz_on_calls++;
}

void buzz_off(void) {
    buzz_off_calls++;
}

void print_timestamp(void) {
    print_timestamp_calls++;
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t len = strlen(captured_printf_output);
    vsnprintf(captured_printf_output + len, sizeof(captured_printf_output) - len, fmt, args);
    va_end(args);
    return 0; // test-only, so we don't care about the return value
}

bool is_motion_detected(void) { return mock_motion_detected; }
bool is_alarm_armed(void) { return mock_alarm_armed; }

void set_alarm_triggered(bool triggered) { 
    calls_set_alarm_triggered++; // <-- MATCH THIS TO YOUR TEST
    set_alarm_triggered_last_param = triggered; 
    mock_alarm_triggered = triggered; 
}

void set_motion_detected(bool detected) { 
    motion_detected_set_calls++; 
    motion_detected_last_value = detected; 
    mock_motion_detected = detected; 
}

// ---- RFID/check_rfid task mocks ----
rfid_tag_t mfrc522_get_tag(void) { return mock_tag; }

bool is_authorized_tag(const rfid_tag_t *tag) { 
    (void)tag; 
    return mock_is_authorized_tag_result; 
}

void toggle_alarm_armed(void) { 
    calls_toggle_alarm_armed++; 
    mock_alarm_armed = !mock_alarm_armed;
}

void buzz_tone_seq(buzzer_sequence_t sequence) {
    calls_buzz_tone_seq++;
    buzz_tone_seq_last_param = sequence;
}

void set_wait_rfid(bool wait) {
    calls_set_wait_rfid++;
    set_wait_rfid_last_param = wait;
    mock_wait_rfid = wait;
}

void reset_rfid_scan_state(void) {
    last_scan_time = 0;
}

// ---- Sleep check mocks ----
void rtos_delay_ms(uint32_t ms) {
    rtos_delay_ms_calls++;
    rtos_delay_ms_last = ms;
}

void enter_sleep_mode(void) {
    enter_sleep_mode_calls++;
}

bool is_wait_rfid(void) {
    return mock_wait_rfid;
}

// ---- Reset helpers for tests ----
void reset_bt_alert_state(void) {
    mock_alarm_triggered = false;
    print_timestamp_calls = 0;
    captured_printf_output[0] = '\0';
    // Add any other BT alert mock state resets here
}

void reset_buzzer_alert_state(void) {
    mock_alarm_triggered = false;
    buzz_on_calls = 0;
    buzz_off_calls = 0;
    mock_clock = 0;
    // Add any other buzzer alert mock state resets here
}

void reset_detect_motion_mocks(void) {
    mock_motion_detected = false;
    mock_alarm_armed = false;
    mock_alarm_triggered = false;
    calls_set_alarm_triggered = 0;
    set_alarm_triggered_last_param = false;
    motion_detected_set_calls = 0;
    motion_detected_last_value = false;
}

// ---- RFID/check_rfid reset helper ----
void reset_check_rfid_mocks(void) {
    mock_clock_count = 0;
    for (int i = 0; i < UID_LENGTH; ++i) mock_tag.uid[i] = 0;
    mock_is_authorized_tag_result = false;
    calls_toggle_alarm_armed = 0;
    calls_buzz_tone_seq = 0;
    buzz_tone_seq_last_param = 0;
    calls_set_alarm_triggered = 0;
    set_alarm_triggered_last_param = false;
    calls_set_wait_rfid = 0;
    set_wait_rfid_last_param = false;
    mock_alarm_armed = false;
    mock_alarm_triggered = false;
}

// ---- Sleep check reset helper ----
void reset_sleep_check_mocks(void) {
    wait_rfid_start_time = 0;
    armed_wake_time = 0;
    in_sleep_mode = false;
    rtos_delay_ms_calls = 0;
    rtos_delay_ms_last = 0;
    enter_sleep_mode_calls = 0;
    mock_wait_rfid = false;
    set_wait_rfid_calls = 0;
    set_wait_rfid_last = false;
}