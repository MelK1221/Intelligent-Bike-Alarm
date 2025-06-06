#include "mpu6050.h"
#include "mpu6050_helper.h"
#include "reg_options.h"
#include "controller_state.h"
#include "rtos.h"
#include <math.h>
#include "uart.h"

#define THRESHOLD 1.5f
#define ACCEL_SCALE 4096.0f
#define STATIONARY_G 1.0f

void detect_motion(void) {
    static bool motion_reported = false;
    accel_data_t offset = get_calibration_offset();

accel_data_t raw_accel_data = mpu6050_read_real_accel();

accel_data_t corrected = {
    .x = raw_accel_data.x - offset.x,
    .y = raw_accel_data.y - offset.y,
    .z = raw_accel_data.z - offset.z
};

    float tot_accel = sqrt(corrected.x * corrected.x + corrected.y * corrected.y + corrected.z * corrected.z);

    if (fabsf(tot_accel - STATIONARY_G) > THRESHOLD) {
        if (!motion_reported) {
            printf("Motion detected: ax=%.2f ay=%.2f az=%.2f total=%.2f\r\n", corrected.x, corrected.y, corrected.z, tot_accel);
            printf("At clock count: %lu\r\n", rtos_clock_count);
            motion_reported = true;
        }
    } else {
        // Reset flag when no motion detected
        motion_reported = false;
    }
}