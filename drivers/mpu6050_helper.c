#include "mpu6050.h"
#include "reg_options.h"
#include "mpu6050_helper.h"
#include "twi_master.h"
#include "rtos.h"
#include <avr/sleep.h>
#include <stdio.h>

#define MPU6050_ADDR 0x68  // default I2C address for MPU6050

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
