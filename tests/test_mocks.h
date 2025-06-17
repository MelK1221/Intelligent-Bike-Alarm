#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "buzzer.h"
#include "rfid.h"

// -------------------
// General Alarm/Motion/Buzzer mocks
// -------------------
extern bool mock_alarm_triggered;
extern uint32_t mock_clock;
extern int buzz_on_calls;
extern int buzz_off_calls;
extern int print_timestamp_calls;
extern char captured_printf_output[1024];
extern bool mock_motion_detected;
extern bool mock_alarm_armed;
extern int alarm_triggered_set_calls;
extern bool alarm_triggered_last_value;
extern int motion_detected_set_calls;
extern bool motion_detected_last_value;

// -------------------
// RFID and check_rfid task mocks
// -------------------
extern rfid_tag_t mock_tag;
extern bool mock_is_authorized_tag_result;
extern int calls_toggle_alarm_armed;
extern int calls_buzz_tone_seq;
extern buzzer_sequence_t buzz_tone_seq_last_param;
extern int calls_set_alarm_triggered;
extern bool set_alarm_triggered_last_param;
extern int calls_set_wait_rfid;
extern bool set_wait_rfid_last_param;
extern uint32_t mock_clock_count; // For rtos_get_clock_count
extern uint32_t last_scan_time;

// -------------------
// Sleep check task mocks/globals
// -------------------
extern uint32_t wait_rfid_start_time;
extern uint32_t armed_wake_time;
extern volatile bool in_sleep_mode;
extern int rtos_delay_ms_calls;
extern uint32_t rtos_delay_ms_last;
extern int enter_sleep_mode_calls;
extern bool mock_wait_rfid;
extern int set_wait_rfid_calls;
extern bool set_wait_rfid_last;

// -------------------
// Mocked function declarations
// -------------------
bool is_alarm_triggered(void);
uint32_t rtos_get_clock_count(void);
void buzz_on(void);
void buzz_off(void);
void print_timestamp(void);
bool is_motion_detected(void);
bool is_alarm_armed(void);
void set_alarm_triggered(bool triggered);
void set_motion_detected(bool detected);

// RFID/check_rfid mocks
rfid_tag_t mfrc522_get_tag(void);
bool is_authorized_tag(const rfid_tag_t *tag);
void toggle_alarm_armed(void);
void buzz_tone_seq(buzzer_sequence_t sequence);
void set_wait_rfid(bool wait);
void reset_rfid_scan_state(void);

// Sleep check mocks
void rtos_delay_ms(uint32_t ms);
void enter_sleep_mode(void);
bool is_wait_rfid(void);

// -------------------
// Reset functions for tests
// -------------------
void reset_bt_alert_state(void);
void reset_buzzer_alert_state(void);
void reset_detect_motion_mocks(void);
void reset_check_rfid_mocks(void);
void reset_sleep_check_mocks(void);