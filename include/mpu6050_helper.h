// Accelerometer (MPU6050) module helper functions header file

#ifndef MPU6050_HELPER_H
#define MPU6050_HELPER_H

#include <stdio.h>
#include <stddef.h>
#include <string.h> 
#include <stdint.h>
#include <avr/interrupt.h>

#include "mpu6050.h"
//#include "sleep.h"
#include "reg_options.h"
#include "twi_master.h"
#include "controller_state.h"
#include "rtos.h"

// typedef struct {
//     float x;
//     float y;
//     float z;
// } dim3_data_t;

// typedef dim3_data_t accel_data_t;
// typedef dim3_data_t gyro_data_t;

int mpu6050_read_reg(uint8_t reg, uint8_t *data);
int mpu6050_write_reg(uint8_t reg, uint8_t data);
// int mpu6050_read_burst(uint16_t reg, uint8_t *data, uint16_t len);
// accel_data_t mpu6050_read_accel(void);
// gyro_data_t mpu6050_read_gyro(void);
// void calibrate_accelerometer(uint16_t sample_count);
// void calibrate_gyro(uint16_t sample_count);
// accel_data_t get_accel_calibration(void);
// gyro_data_t get_gyro_calibration(void);
int init_mpu6050_w_interrupt(void);

#endif // MPU6050_HELPER_H
