#include "controller_state.h"
#include "buzzer.h"
#include "uart.h"
#include "rtos.h"
#include "mpu6050_helper.h"
#include <math.h>

#define MOTION_VERIFY_SAMPLES    40      // 2 seconds with 50ms interval
#define MOTION_VERIFY_DELAY_MS   50
#define ACCEL_THRESHOLD_G        0.005f
#define GYRO_THRESHOLD_DPS       1.0f
#define REQUIRED_CONSISTENT_SAMPLES 24   // 75% of 40

void detect_motion(void) {
    if (is_motion_detected()) {
        if (is_alarm_armed() && !is_alarm_triggered()) {
            int accel_significant = 0;
            int gyro_significant  = 0;
            accel_data_t accel_offset = get_accel_calibration();
            gyro_data_t  gyro_offset  = get_gyro_calibration();

            for (int i = 0; i < MOTION_VERIFY_SAMPLES; ++i) {
                // Accelerometer readings (with offset)
                accel_data_t a = mpu6050_read_accel();
                float ax = a.x - accel_offset.x;
                float ay = a.y - accel_offset.y;
                float az = a.z - accel_offset.z;
                float accel_mag = sqrtf(ax * ax + ay * ay + az * az);

                if (fabsf(accel_mag - 1.0f) > ACCEL_THRESHOLD_G) {
                    accel_significant++;
                }

                // Gyroscope readings (with offset)
                gyro_data_t g = mpu6050_read_gyro();
                float gx = g.x - gyro_offset.x;
                float gy = g.y - gyro_offset.y;
                float gz = g.z - gyro_offset.z;
                float gyro_mag = sqrtf(gx * gx + gy * gy + gz * gz);

                if (gyro_mag > GYRO_THRESHOLD_DPS) {
                    gyro_significant++;
                }

                rtos_delay_ms(MOTION_VERIFY_DELAY_MS);
            }

            if ((accel_significant >= REQUIRED_CONSISTENT_SAMPLES) ||
                (gyro_significant  >= REQUIRED_CONSISTENT_SAMPLES)) {
                set_alarm_triggered(true);
                DEBUG_PRINT("Alarm triggered: consistent accel/gyro motion for 2s.\n");
            } else {
                DEBUG_PRINT("Motion detected, but not consistent enough to trigger alarm.\n");
            }
        }
        // Always clear the flag at the end
        //set_motion_detected(false);
    }
}


// #include "controller_state.h"
// #include "buzzer.h"
// #include "uart.h"
// #include "rtos.h"


// // Example: task runs every ~100ms
// void detect_motion(void) {
//     // Check if motion was detected (set by ISR)
//     if (is_motion_detected()) {
//         //DEBUG_PRINT("Motion detected!\n");

//         // Only trigger alarm if system is armed and alarm not already triggered
//         if (is_alarm_armed() && !is_alarm_triggered()) {
//             set_alarm_triggered(true);
//             //buzz_tone_seq(ALARM_BUZZ);
//             //DEBUG_PRINT("Alarm triggered due to motion!\n");
//         }

//         // Clear the motion flag so we only handle once per event.
//         //set_motion_detected(false);
//     }
// }