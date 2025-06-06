#ifndef MPU6050_HELPER_H
#define MPU6050_HELPER_H

#include <stdint.h>

typedef struct {
    float x;
    float y;
    float z;
} accel_data_t;

typedef struct {
    float x;
    float y;
    float z;
} gyro_data_t;

int mpu6050_read_reg(uint8_t reg, uint8_t *data);
int mpu6050_write_reg(uint8_t reg, uint8_t data);
int mpu6050_read_burst(uint16_t reg, uint8_t *data, uint16_t len);
void delay_ms(uint32_t ms);
void mpu6050_read_raw(int16_t *ax, int16_t *ay, int16_t *az);
accel_data_t mpu6050_read_real_accel(void);
void calibrate_mpu6050(uint16_t sample_count);
accel_data_t get_calibration_offset(void);
gyro_data_t mpu6050_read_gyro(void);
void set_gyro_calibration(gyro_data_t offset);
gyro_data_t get_gyro_calibration(void);
void calibrate_gyro(void);

extern float accel_x_offset;
extern float accel_y_offset;
extern float accel_z_offset;

#endif
