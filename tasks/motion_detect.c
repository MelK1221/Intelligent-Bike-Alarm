#include <math.h>
#include "mpu6050.h"
#include "mpu6050_helper.h"
#include "reg_options.h"
#include "controller_state.h"
#include "uart.h"
#include "rtos.h"

#define ACCEL_THRESHOLD 1.5f
#define GYRO_THRESHOLD 5.0f 
#define ACCEL_SCALE 4096.0f
#define STATIONARY_G 1.0f

void detect_motion(void) {
    static bool initial_motion_detected = false;
    static uint32_t motion_start_time = 0;

    if (!is_alarm_armed()) {
        set_motion_detected(false);
        initial_motion_detected = false;
        return;
    }

    accel_data_t offset = get_accel_calibration();
    gyro_data_t gyro_offset = get_gyro_calibration();

    accel_data_t raw_accel_data = mpu6050_read_accel();
    gyro_data_t raw_gyro_data = mpu6050_read_gyro();

    accel_data_t corrected_a = {
        .x = raw_accel_data.x - offset.x,
        .y = raw_accel_data.y - offset.y,
        .z = raw_accel_data.z - offset.z
    };

    gyro_data_t corrected_g = {
        .x = raw_gyro_data.x - gyro_offset.x,
        .y = raw_gyro_data.y - gyro_offset.y,
        .z = raw_gyro_data.z - gyro_offset.z
    };

    float total_accel = sqrtf(corrected_a.x * corrected_a.x +
                              corrected_a.y * corrected_a.y +
                              corrected_a.z * corrected_a.z);

    bool accel_motion = fabsf(total_accel - STATIONARY_G) > ACCEL_THRESHOLD;
    bool gyro_motion = fabsf(corrected_g.x) > GYRO_THRESHOLD ||
                       fabsf(corrected_g.y) > GYRO_THRESHOLD ||
                       fabsf(corrected_g.z) > GYRO_THRESHOLD;

    bool motion_detected_now = accel_motion || gyro_motion;

    if (motion_detected_now) {
        if (!initial_motion_detected) {
            motion_start_time = rtos_clock_count;
            initial_motion_detected = true;
            printf("Motion detected.");
        } else {
            // Check if 3 seconds have passed
            if ((rtos_clock_count - motion_start_time) >= 3000) {
                if (!is_alarm_triggered()) {
                    set_alarm_triggered(true);
                    printf("Alarm TRIGGERED by motion!");
                }
                set_motion_detected(true);
                initial_motion_detected = false;  // lock out re-trigger
            }
        }
    } else {
        initial_motion_detected = false;
        set_motion_detected(false);
    }
}