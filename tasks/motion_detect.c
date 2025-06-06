#include "mpu6050.h"
#include "mpu6050_helper.h"
#include "reg_options.h"
#include "controller_state.h"
#include "rtos.h"
#include <math.h>
#include "uart.h"

#define THRESHOLD 1.5f
#define GYRO_THRESHOLD 5.0f 
#define ACCEL_SCALE 4096.0f
#define STATIONARY_G 1.0f

void detect_motion(void) {
    static bool motion_reported = false;
    accel_data_t offset = get_calibration_offset();
    gyro_data_t gyro_offset = get_gyro_calibration();

accel_data_t raw_accel_data = mpu6050_read_real_accel();
gyro_data_t raw_gyro_data = mpu6050_read_gyro();

accel_data_t corrected = {
    .x = raw_accel_data.x - offset.x,
    .y = raw_accel_data.y - offset.y,
    .z = raw_accel_data.z - offset.z
};

gyro_data_t corrected_g = {
    .x = raw_gyro_data.x - gyro_offset.x,
    .y = raw_gyro_data.y - gyro_offset.y,
    .z = raw_gyro_data.z - gyro_offset.z
};

    float tot_accel = sqrt(corrected.x * corrected.x + corrected.y * corrected.y + corrected.z * corrected.z);

    bool accel_motion = fabsf(tot_accel - 1.0f) > THRESHOLD;
    bool gyro_motion = (fabsf(corrected_g.x) > GYRO_THRESHOLD) ||
                       (fabsf(corrected_g.y) > GYRO_THRESHOLD) ||
                       (fabsf(corrected_g.z) > GYRO_THRESHOLD);

    if (accel_motion || gyro_motion) {
        if (!motion_reported) {
            printf("Motion detected:\n");
            printf("  Accel: ax=%.2f ay=%.2f az=%.2f total=%.2f\n", corrected.x, corrected.y, corrected.z, tot_accel);
            printf("  Gyro: gx=%.2f gy=%.2f gz=%.2f\n", corrected_g.x, corrected_g.y, corrected_g.z);
            motion_reported = true;
        }
    } else {
        motion_reported = false;
    }
}