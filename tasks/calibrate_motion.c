#include "mpu6050_helper.h"
#include "mpu6050.h"
#include "uart.h"
#include <math.h>
#include "reg_options.h"
#include "controller_state.h"
#include "rtos.h"

#define ACCEL_SCALE 4096.0f  // adjust if your scale differs

void calibrate_motion(void) {
    static uint16_t count = 0;

    if (count++ >= 100) {
        float accel_x = read_accel_axis(REG_ACCEL_X_MEASURE_1, ACCEL_SCALE, mpu6050_read_reg);
        float accel_y = read_accel_axis(REG_ACCEL_Y_MEASURE_1, ACCEL_SCALE, mpu6050_read_reg);
        float accel_z = read_accel_axis(REG_ACCEL_Z_MEASURE_1, ACCEL_SCALE, mpu6050_read_reg);

        float total_accel = sqrt(accel_x*accel_x + accel_y*accel_y + accel_z*accel_z);

        printf("Accel X: %.3f g, Y: %.3f g, Z: %.3f g, Total: %.3f g\r\n", accel_x, accel_y, accel_z, total_accel);

        count = 0;
    }
}
