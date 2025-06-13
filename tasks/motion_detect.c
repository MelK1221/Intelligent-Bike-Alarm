// #include "controller_state.h"
// #include "buzzer.h"
// #include "uart.h"
// #include "rtos.h"
// #include "mpu6050_helper.h"
// #include <math.h>

// // --- Tuning parameters ---
// #define MOTION_VERIFY_SAMPLES        80      //    4 seconds with 50ms interval
// #define MOTION_VERIFY_DELAY_MS       50
// #define ACCEL_VARIANCE_THRESHOLD     0.00015f // Sensitive: adjust as needed (g^2)
// #define GYRO_VARIANCE_THRESHOLD      0.15f    // Sensitive: adjust as needed (deg/s)^2

// void detect_motion(void) {
//     if (is_motion_detected()) {
//         if (is_alarm_armed() && !is_alarm_triggered()) {
//             accel_data_t accel_offset = get_accel_calibration();
//             gyro_data_t  gyro_offset  = get_gyro_calibration();

//             float accel_mag[MOTION_VERIFY_SAMPLES];
//             float gyro_mag[MOTION_VERIFY_SAMPLES];

//             // Gather samples for both sensors
//             for (int i = 0; i < MOTION_VERIFY_SAMPLES; ++i) {
//                 // --- Accelerometer magnitude, gravity-subtracted ---
//                 accel_data_t a = mpu6050_read_accel();
//                 float ax = a.x - accel_offset.x;
//                 float ay = a.y - accel_offset.y;
//                 float az = a.z - accel_offset.z;
//                 float am = sqrtf(ax * ax + ay * ay + az * az);
//                 accel_mag[i] = am;

//                 // --- Gyroscope magnitude ---
//                 gyro_data_t g = mpu6050_read_gyro();
//                 float gx = g.x - gyro_offset.x;
//                 float gy = g.y - gyro_offset.y;
//                 float gz = g.z - gyro_offset.z;
//                 float gm = sqrtf(gx * gx + gy * gy + gz * gz);
//                 gyro_mag[i] = gm;

//                 rtos_delay_ms(MOTION_VERIFY_DELAY_MS);
//             }

//             // --- Calculate variance for accel ---
//             float accel_sum = 0.0f, accel_var = 0.0f;
//             for (int i = 0; i < MOTION_VERIFY_SAMPLES; ++i) {
//                 accel_sum += accel_mag[i];
//             }
//             float accel_mean = accel_sum / MOTION_VERIFY_SAMPLES;
//             for (int i = 0; i < MOTION_VERIFY_SAMPLES; ++i) {
//                 float d = accel_mag[i] - accel_mean;
//                 accel_var += d * d;
//             }
//             accel_var /= MOTION_VERIFY_SAMPLES;

//             // --- Calculate variance for gyro ---
//             float gyro_sum = 0.0f, gyro_var = 0.0f;
//             for (int i = 0; i < MOTION_VERIFY_SAMPLES; ++i) {
//                 gyro_sum += gyro_mag[i];
//             }
//             float gyro_mean = gyro_sum / MOTION_VERIFY_SAMPLES;
//             for (int i = 0; i < MOTION_VERIFY_SAMPLES; ++i) {
//                 float d = gyro_mag[i] - gyro_mean;
//                 gyro_var += d * d;
//             }
//             gyro_var /= MOTION_VERIFY_SAMPLES;

//             // --- Decision logic: trigger if either variance exceeds threshold ---
//             if ((accel_var > ACCEL_VARIANCE_THRESHOLD) ||
//                 (gyro_var > GYRO_VARIANCE_THRESHOLD)) {
//                 set_alarm_triggered(true);
//                 // buzz_tone_seq(ALARM_BUZZ);
//                 DEBUG_PRINT("Alarm triggered: variance exceeded (accel: %.6f, gyro: %.4f)\n", accel_var, gyro_var);
//             } else {
//                 DEBUG_PRINT("Motion detected, but variance too low (accel: %.6f, gyro: %.4f)\n", accel_var, gyro_var);
//             }
//         }
//         //set_motion_detected(false);
//     }
// }


#include "controller_state.h"
#include "buzzer.h"
#include "uart.h"
#include "rtos.h"


// Example: task runs every ~100ms
void detect_motion(void) {
    // Check if motion was detected (set by ISR)
    if (is_motion_detected()) {
        //DEBUG_PRINT("Motion detected!\n");

        // Only trigger alarm if system is armed and alarm not already triggered
        if (is_alarm_armed() && !is_alarm_triggered()) {
            set_alarm_triggered(true);
            //buzz_tone_seq(ALARM_BUZZ);
            //DEBUG_PRINT("Alarm triggered due to motion!\n");
        }

        // Clear the motion flag so we only handle once per event.
        //set_motion_detected(false);
    }
}