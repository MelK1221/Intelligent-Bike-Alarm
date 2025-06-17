/* Accelerometer module (MPU6050) driver
* Adapted from https://github.com/KadePerrotti/MPU6050-Driver.git
* Platform independent MPU6050 Driver library by Kade Perrotti
*/

/*
 * MPU6050.c
 *
 *  Created on: Nov 25, 2024
 *      Author: Kade Perrotti
 * Implementations of functions that initialize and read data from
 * the MPU6050
 */
#include "mpu6050.h"

uint16_t init_mpu6050(MPU6050_REG_WRITE_TYPE writeReg, DELAY_MS_TYPE delay)
{
    //DEBUG_PRINT("MPU6050: Resetting power management...\n");
    if (writeReg(REG_PWR_MGMT_1, PWR_MGMT_DEV_RESET) != 0) {
        DEBUG_PRINT("ERROR: Failed to write REG_PWR_MGMT_1\n");
        return 1;
    }
    rtos_delay_ms(100);

    //DEBUG_PRINT("MPU6050: Resetting signal paths...\n");
    if (writeReg(REG_USER_CTRL, I2C_MST_RESET | SIG_COND_RESET) != 0) {
        DEBUG_PRINT("ERROR: Failed to write REG_USER_CTRL\n");
        return 2;
    }
    rtos_delay_ms(100);

    //DEBUG_PRINT("MPU6050: Setting clock source...\n");
    if (writeReg(REG_PWR_MGMT_1, PWR_MGMT_CLK_SEL_INTERNAL) != 0) {
        DEBUG_PRINT("ERROR: Failed to set clock source\n");
        return 3;
    }

    //DEBUG_PRINT("MPU6050: Awakening sensors...\n");
    if (writeReg(REG_PWR_MGMT_2, 0x00) != 0) {
        DEBUG_PRINT("ERROR: Failed to write REG_PWR_MGMT_2\n");
        return 4;
    }

    rtos_delay_ms(100);

    // === Motion detection configuration ===
    if (writeReg(REG_ACCEL_CONFIG, 0x00) != 0) return 5;

    if (writeReg(REG_MOT_THR, 0x01) != 0) return 6;

    if (writeReg(REG_MOT_DUR, 0x01) != 0) return 7;

    if (writeReg(REG_MOT_DETECT_CTRL, 0x15) != 0) return 8;

    if (writeReg(REG_INT_PIN_CFG, 0x10) != 0) return 9;

    if (writeReg(REG_INT_ENABLE, 0x40) != 0) return 10;

    delay(1000);

    return 0;
}

int16_t read_raw_axis(uint8_t address, MPU6050_REG_READ_TYPE readReg)
{
    uint8_t measureUpper = 0;
    uint8_t measureLower = 0;
    
    //upper portion of accel 
    readReg(address, &measureUpper);
    
    //lower portion
    readReg(address + 1, &measureLower);
    
    int16_t combined = (int16_t)(measureUpper << 8) | measureLower;
    return combined;
}

uint16_t read_fifo_count(MPU6050_REG_READ_TYPE readReg)
{
    uint8_t upper = 0;
    uint8_t lower = 0;
    readReg(REG_FIFO_COUNT_H, &upper);
    readReg(REG_FIFO_COUNT_L, &lower);
    uint16_t combined = (((uint16_t)upper) << 8) | lower;
    return combined;
}
