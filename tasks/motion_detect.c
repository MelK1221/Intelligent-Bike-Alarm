#include "mpu6050.h"
#include "mpu6050_helper.h"
#include "reg_options.h"
#include "controller_state.h"
#include "rtos.h"
#include <math.h>
#include "uart.h"

#define THRESHOLD 1.5f
#define ACCEL_SCALE 4096.0f

void detect_motion(void) {
    static bool motion_reported = false;

    float accel_x = read_accel_axis(REG_ACCEL_X_MEASURE_1, ACCEL_SCALE, mpu6050_read_reg);
    float accel_y = read_accel_axis(REG_ACCEL_Y_MEASURE_1, ACCEL_SCALE, mpu6050_read_reg);
    float accel_z = read_accel_axis(REG_ACCEL_Z_MEASURE_1, ACCEL_SCALE, mpu6050_read_reg);

    float tot_accel = sqrt(accel_x * accel_x + accel_y * accel_y + accel_z * accel_z);

    if (fabsf(tot_accel - 1.0f) > THRESHOLD) {
        if (!motion_reported) {
            printf("Motion detected: ax=%.2f ay=%.2f az=%.2f total=%.2f\r\n", accel_x, accel_y, accel_z, tot_accel);
            printf("At clock count: %lu\r\n", rtos_clock_count);
            motion_reported = true;
        }
    } else {
        // Reset flag when no motion detected
        motion_reported = false;
    }
}