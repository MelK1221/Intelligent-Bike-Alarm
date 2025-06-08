#ifndef MPU6050_HELPER_H
#define MPU6050_HELPER_H

#include <stdint.h>

typedef struct {
    float x;
    float y;
    float z;
} 3d_data_t;

typedef 3d_data_t accel_data_t;
typedef 3d_data_t gyro_data_t;

int mpu6050_read_reg(uint8_t reg, uint8_t *data);
int mpu6050_write_reg(uint8_t reg, uint8_t data);
int mpu6050_read_burst(uint16_t reg, uint8_t *data, uint16_t len);
accel_data_t mpu6050_read_accel(void);
gyro_data_t mpu6050_read_gyro(void);
void calibrate_accelerometer(uint16_t sample_count);
void calibrate_gyro(uint16_t sample_count);
accel_data_t get_accel_calibration(void);
gyro_data_t get_gyro_calibration(void);
int init_mpu6050_w_cal(void);

#endif
