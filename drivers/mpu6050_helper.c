#include "mpu6050_helper.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h> 
#include <stdint.h>
#include <avr/interrupt.h>

#include "mpu6050.h"
#include "sleep.h"
#include "reg_options.h"
#include "twi_master.h"
#include "controller_state.h"
#include "rtos.h"

#define ACCEL_SCALE 4096.0f
#define GYRO_SCALE 131.0f
#define CALIBRATION_DELAY_MS 10
#define ACCEL_START_REG 0x3B
#define GYRO_START_REG 0x43
#define MPU6050_BURST_LEN 6


// Calibration offsets (initialized to 0)
static accel_data_t accel_offset = {0};
static gyro_data_t gyro_offset = {0};


int mpu6050_read_reg(uint8_t reg, uint8_t *data) {
    uint8_t reg_addr = (uint8_t)reg;
    if (tw_master_transmit(MPU_6050_ADDR, &reg_addr, 1, true) != SUCCESS)
        return -1;
    if (tw_master_receive(MPU_6050_ADDR, data, 1) != SUCCESS)
        return -1;
    return 0;
}

int mpu6050_write_reg(uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    //DEBUG_PRINT("Writing reg 0x%02X with data 0x%02X\n", reg, data);
    int ret = tw_master_transmit(MPU_6050_ADDR, buf, 2, false);
    //DEBUG_PRINT("tw_master_transmit returned %d\n", ret);
    if (ret != SUCCESS) {
        DEBUG_PRINT("Write failed\n");
        return -1;
    }
    return 0;
}

int mpu6050_read_burst(uint16_t reg, uint8_t *data, uint16_t len) {
    uint8_t reg_addr = (uint8_t)reg;
    if (tw_master_transmit(MPU_6050_ADDR, &reg_addr, 1, true) != SUCCESS)
        return -1;
    if (tw_master_receive(MPU_6050_ADDR, data, len) != SUCCESS)
        return -1;
    return 0;
}

accel_data_t mpu6050_read_accel(void) {
    uint8_t raw_data[6];
    accel_data_t accel = {0};

    // Read 6 bytes from ACCEL_XOUT_H to ACCEL_ZOUT_L
    if (mpu6050_read_burst(ACCEL_START_REG, raw_data, MPU6050_BURST_LEN) != 0) {
        DEBUG_PRINT("Error reading accelerometer data\n");
        return accel;
    }

    int16_t raw_x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
    int16_t raw_y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
    int16_t raw_z = (int16_t)((raw_data[4] << 8) | raw_data[5]);

    accel.x = (float)raw_x / ACCEL_SCALE;
    accel.y = (float)raw_y / ACCEL_SCALE;
    accel.z = (float)raw_z / ACCEL_SCALE;

    return accel;
}

gyro_data_t mpu6050_read_gyro(void) {
    uint8_t raw_data[6];
    gyro_data_t gyro = {0};

    // Read 6 bytes from GYRO_XOUT_H to GYRO_ZOUT_L
    if (mpu6050_read_burst(GYRO_START_REG, raw_data, MPU6050_BURST_LEN) != 0) {
        DEBUG_PRINT("Error reading gyroscope data\n");
        return gyro;
    }

    int16_t raw_x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
    int16_t raw_y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
    int16_t raw_z = (int16_t)((raw_data[4] << 8) | raw_data[5]);

    gyro.x = (float)raw_x / GYRO_SCALE;
    gyro.y = (float)raw_y / GYRO_SCALE;
    gyro.z = (float)raw_z / GYRO_SCALE;

    return gyro;
}

void calibrate_accelerometer(uint16_t sample_count) {
    accel_data_t sum = {0};
    uint16_t successful_reads = 0;
    for (uint16_t i = 0; i < sample_count; i++) {
        accel_data_t sample = mpu6050_read_accel();
        if (sample.x == 0.0f && sample.y == 0.0f && sample.z == 0.0f) {
            continue;  // skip failed read
        }
        sum.x += sample.x;
        sum.y += sample.y;
        sum.z += sample.z;
        successful_reads++;
        rtos_delay_ms(CALIBRATION_DELAY_MS);
    }

    if (successful_reads == 0) {
        DEBUG_PRINT("Accelerometer calibration failed: no valid samples.\n");
        return;
    }
    accel_offset.x = sum.x / successful_reads;
    accel_offset.y = sum.y / successful_reads;
    accel_offset.z = (sum.z / successful_reads) - 1.0f; 

    DEBUG_PRINT("Accelerometer calibration complete: Offset X: %.3f, Y: %.3f, Z: %.3f\n", accel_offset.x, accel_offset.y, accel_offset.z);
}

void calibrate_gyro(uint16_t sample_count) {
    gyro_data_t sum = {0};
    uint16_t successful_reads = 0;

    for (int i = 0; i < sample_count; i++) {
        gyro_data_t sample = mpu6050_read_gyro();
        if (sample.x == 0.0f && sample.y == 0.0f && sample.z == 0.0f) {
            continue;  // skip failed read
        }

        sum.x += sample.x;
        sum.y += sample.y;
        sum.z += sample.z;
        successful_reads++;
        rtos_delay_ms(CALIBRATION_DELAY_MS);
    }

    if (successful_reads == 0) {
        DEBUG_PRINT("Gyro calibration failed: no valid samples.\n");
        return;
    }

    gyro_offset.x = sum.x / sample_count;
    gyro_offset.y = sum.y / sample_count;
    gyro_offset.z = sum.z / sample_count;

    DEBUG_PRINT("Gyro calibration complete: Offset X: %.3f, Y: %.3f, Z: %.3f\n", gyro_offset.x, gyro_offset.y, gyro_offset.z);

}

accel_data_t get_accel_calibration(void) {
    return accel_offset;
}

gyro_data_t get_gyro_calibration(void) {
    return gyro_offset;
}

ISR(INT0_vect) {
    PORTB ^= (1 << PB5); // Toggle LED
   // DEBUG_PRINT("INT0: Motion detected interrupt.\n");
    set_motion_detected(true);
}

void setup_motion_interrupt(void) {
    cli();  // Disable interrupts during config

    // Configure INT0 (PD2, Arduino pin 2) for rising edge trigger
    EICRA |= (1 << ISC01) | (1 << ISC00);  // ISC01=1, ISC00=1 -> Rising edge
    EIMSK |= (1 << INT0);                  // Enable external interrupt INT0

    sei();  // Re-enable global interrupts
}

int init_mpu6050_w_interrupt(void) {
    if (init_mpu6050(mpu6050_write_reg, rtos_delay_ms) != 0) {
        DEBUG_PRINT("MPU6050 initialization failed.\r\n");
        return -1;
    }
    setup_motion_interrupt();
    calibrate_accelerometer(1000);
    calibrate_gyro(1000);

    DEBUG_PRINT("MPU6050 initialization complete.\r\n");
    return 0;
}




// void mpu6050_read_raw(int16_t *ax, int16_t *ay, int16_t *az)
// {
//     *ax = read_raw_axis(REG_ACCEL_X_MEASURE_1, mpu6050_read_reg);
//     *ay = read_raw_axis(REG_ACCEL_Y_MEASURE_1, mpu6050_read_reg);
//     *az = read_raw_axis(REG_ACCEL_Z_MEASURE_1, mpu6050_read_reg);
// }

// void delay_ms(uint32_t ms) {
//     uint32_t start_tick = rtos_clock_count;
//     // Assuming rtos_clock_count increments every 10 ms
//     uint32_t wait_ticks = ms / 10;
//     if (wait_ticks == 0) wait_ticks = 1;  // Minimum 1 tick wait

//     while ((rtos_clock_count - start_tick) < wait_ticks) {
//         //sleep_cpu();
//         //printf("Hello!");
//     }
// }
// // Helper to read a 16-bit signed value from two registers
// static int16_t mpu6050_read_16bit(uint8_t reg_high, uint8_t reg_low) {
//     uint8_t high_byte, low_byte;
//     if (mpu6050_read_reg(reg_high, &high_byte) != 0) {
//         return 0; // or handle error as you prefer
//     }
//     if (mpu6050_read_reg(reg_low, &low_byte) != 0) {
//         return 0; // or handle error
//     }
//     return (int16_t)(((uint16_t)high_byte << 8) | low_byte);
// }
