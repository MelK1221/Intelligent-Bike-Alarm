#include "mpu6050_helper.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h> 
#include <stdint.h>
#include <avr/sleep.h>
#include "mpu6050.h"
#include "reg_options.h"
#include "twi_master.h"
#include "rtos.h"

#define MPU6050_ADDR 0x68  // default I2C address for MPU6050
#define ACCEL_SCALE 4096.0f
#define CALIBRATION_SAMPLES 1000

// Calibration offsets (initialized to 0)
static accel_data_t calibration_offset = {0};
static gyro_data_t gyro_offset = {0};


int mpu6050_read_reg(uint8_t reg, uint8_t *data) {
    uint8_t reg_addr = (uint8_t)reg;
    if (tw_master_transmit(MPU6050_ADDR, &reg_addr, 1, true) != SUCCESS)
        return -1;
    if (tw_master_receive(MPU6050_ADDR, data, 1) != SUCCESS)
        return -1;
    return 0;
}

int mpu6050_write_reg(uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    printf("Writing reg 0x%02X with data 0x%02X\n", reg, data);
    int ret = tw_master_transmit(MPU6050_ADDR, buf, 2, false);
    printf("tw_master_transmit returned %d\n", ret);
    if (ret != SUCCESS) {
        printf("Write failed\n");
        return -1;
    }
    return 0;
}

int mpu6050_read_burst(uint16_t reg, uint8_t *data, uint16_t len) {
    uint8_t reg_addr = (uint8_t)reg;
    if (tw_master_transmit(MPU6050_ADDR, &reg_addr, 1, true) != SUCCESS)
        return -1;
    if (tw_master_receive(MPU6050_ADDR, data, len) != SUCCESS)
        return -1;
    return 0;
}

void mpu6050_read_raw(int16_t *ax, int16_t *ay, int16_t *az)
{
    *ax = read_raw_axis(REG_ACCEL_X_MEASURE_1, mpu6050_read_reg);
    *ay = read_raw_axis(REG_ACCEL_Y_MEASURE_1, mpu6050_read_reg);
    *az = read_raw_axis(REG_ACCEL_Z_MEASURE_1, mpu6050_read_reg);
}

void delay_ms(uint32_t ms) {
    uint32_t start_tick = rtos_clock_count;
    // Assuming rtos_clock_count increments every 10 ms
    uint32_t wait_ticks = ms / 10;
    if (wait_ticks == 0) wait_ticks = 1;  // Minimum 1 tick wait

    while ((rtos_clock_count - start_tick) < wait_ticks) {
        //sleep_cpu();
        //printf("Hello!");
    }
}

// Helper to read a 16-bit signed value from two registers
static int16_t mpu6050_read_16bit(uint8_t reg_high, uint8_t reg_low) {
    uint8_t high_byte, low_byte;
    if (mpu6050_read_reg(reg_high, &high_byte) != 0) {
        return 0; // or handle error as you prefer
    }
    if (mpu6050_read_reg(reg_low, &low_byte) != 0) {
        return 0; // or handle error
    }
    return (int16_t)((high_byte << 8) | low_byte);
}

// Reads accelerometer data and converts to g's
accel_data_t mpu6050_read_real_accel(void) {
    accel_data_t accel;

    const uint8_t REG_ACCEL_XOUT_H = 0x3B;
    const uint8_t REG_ACCEL_XOUT_L = 0x3C;
    const uint8_t REG_ACCEL_YOUT_H = 0x3D;
    const uint8_t REG_ACCEL_YOUT_L = 0x3E;
    const uint8_t REG_ACCEL_ZOUT_H = 0x3F;
    const uint8_t REG_ACCEL_ZOUT_L = 0x40;

    int16_t raw_x = mpu6050_read_16bit(REG_ACCEL_XOUT_H, REG_ACCEL_XOUT_L);
    int16_t raw_y = mpu6050_read_16bit(REG_ACCEL_YOUT_H, REG_ACCEL_YOUT_L);
    int16_t raw_z = mpu6050_read_16bit(REG_ACCEL_ZOUT_H, REG_ACCEL_ZOUT_L);

    accel.x = (float)raw_x / ACCEL_SCALE;
    accel.y = (float)raw_y / ACCEL_SCALE;
    accel.z = (float)raw_z / ACCEL_SCALE;

    return accel;
}

void calibrate_mpu6050(uint16_t sample_count) {
    accel_data_t sum = {0};
    for (uint16_t i = 0; i < sample_count; i++) {
        accel_data_t data = mpu6050_read_real_accel();
        sum.x += data.x;
        sum.y += data.y;
        sum.z += data.z;
        delay_ms_rtos(5); // Small delay between samples
    }

    calibration_offset.x = sum.x / sample_count;
    calibration_offset.y = sum.y / sample_count;
    calibration_offset.z = (sum.z / sample_count) - 1.0f;  // Remove gravity

    // Optional: print for debugging
    printf("Calibration complete: Offset X: %.3f, Y: %.3f, Z: %.3f\n",
           calibration_offset.x, calibration_offset.y, calibration_offset.z);
}

accel_data_t get_calibration_offset(void) {
    return calibration_offset;
}

gyro_data_t mpu6050_read_gyro(void) {
    uint8_t raw_data[6];
    gyro_data_t gyro = {0};

    for (int i = 0; i < 6; ++i) {
        if (mpu6050_read_reg(0x43 + i, &raw_data[i]) != 0) {
            // Handle error if needed
        }
    }

    int16_t raw_x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
    int16_t raw_y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
    int16_t raw_z = (int16_t)((raw_data[4] << 8) | raw_data[5]);

    const float gyro_scale = 131.0f;  // Assuming FS_SEL=0 ±250 °/s
    gyro.x = (float)raw_x / gyro_scale;
    gyro.y = (float)raw_y / gyro_scale;
    gyro.z = (float)raw_z / gyro_scale;

    return gyro;
}

void set_gyro_calibration(gyro_data_t offset) {
    gyro_offset = offset;
}

gyro_data_t get_gyro_calibration(void) {
    return gyro_offset;
}

void calibrate_gyro(void) {
    int32_t sum_x = 0, sum_y = 0, sum_z = 0;

    for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
        gyro_data_t sample = mpu6050_read_gyro();
        sum_x += (int32_t)(sample.x * 1000);  // scale up to avoid float sum errors
        sum_y += (int32_t)(sample.y * 1000);
        sum_z += (int32_t)(sample.z * 1000);

        // small delay between samples if needed
        delay_ms_rtos(1);
    }

    gyro_data_t offset = {
        .x = sum_x / (float)CALIBRATION_SAMPLES / 1000.0f,
        .y = sum_y / (float)CALIBRATION_SAMPLES / 1000.0f,
        .z = sum_z / (float)CALIBRATION_SAMPLES / 1000.0f
    };

    set_gyro_calibration(offset);
}