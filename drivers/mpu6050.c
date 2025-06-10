/*
 * MPU6050.c
 *
 *  Created on: Nov 25, 2024
 *      Author: Kade Perrotti
 * Implementations of functions that initialize and read data from
 * the MPU6050
 */
#include "mpu6050.h"

#include <stdio.h>
//#include <math.h>

#include "reg_options.h"
#include "rtos.h"


uint16_t init_mpu6050(MPU6050_REG_WRITE_TYPE writeReg, DELAY_MS_TYPE delay)
{
    printf("MPU6050: Resetting power management...\n");
    if (writeReg(REG_PWR_MGMT_1, PWR_MGMT_DEV_RESET) != 0) {
        printf("ERROR: Failed to write REG_PWR_MGMT_1\n");
        return 1;
    }
    rtos_delay_ms(100);

    printf("MPU6050: Resetting signal paths...\n");
    if (writeReg(REG_USER_CTRL, I2C_MST_RESET | SIG_COND_RESET) != 0) {
        printf("ERROR: Failed to write REG_USER_CTRL\n");
        return 2;
    }
    rtos_delay_ms(100);

    printf("MPU6050: Setting clock source...\n");
    if (writeReg(REG_PWR_MGMT_1, PWR_MGMT_CLK_SEL_INTERNAL) != 0) {
        printf("ERROR: Failed to set clock source\n");
        return 3;
    }

    printf("MPU6050: Awakening sensors...\n");
    if (writeReg(REG_PWR_MGMT_2, 0x00) != 0) {
        printf("ERROR: Failed to write REG_PWR_MGMT_2\n");
        return 4;
    }

    delay(100);

    // === Motion detection configuration ===
    printf("MPU6050: Setting accelerometer range to ±2g...\n");
    if (writeReg(REG_ACCEL_CONFIG, 0x00) != 0) return 5;

    printf("MPU6050: Setting motion threshold...\n");
    if (writeReg(REG_MOT_THR, 0x01) != 0) return 6;  // Adjust sensitivity here

    printf("MPU6050: Setting motion duration...\n");
    if (writeReg(REG_MOT_DUR, 0x01) != 0) return 7;

    printf("MPU6050: Enabling motion detection logic...\n");
    if (writeReg(REG_MOT_DETECT_CTRL, 0x15) != 0) return 8;

    printf("MPU6050: Configuring interrupt pin behavior...\n");
    if (writeReg(REG_INT_PIN_CFG, 0x10) != 0) return 9;  // Latch interrupt until INT_STATUS read

    printf("MPU6050: Enabling motion interrupt...\n");
    if (writeReg(REG_INT_ENABLE, 0x40) != 0) return 10;

    delay(1000);
    printf("MPU6050: Initialization and motion config complete.\n");

    return 0;
}


// float read_accel_axis(uint8_t address, uint16_t scaler, MPU6050_REG_READ_TYPE readReg)
// {
//     int16_t rawReading = read_raw_axis(address, readReg);
//     float scaled = (float)rawReading / scaler;
//     return scaled;
// }

// float read_gyro_axis(uint8_t address, uint16_t scaler, MPU6050_REG_READ_TYPE readReg)
// {
//     int16_t rawReading = read_raw_axis(address, readReg);
//     float scaled = (float)rawReading / scaler;
//     return scaled;
// }

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

// FACTORY_TEST_RESULTS gyro_self_test(MPU6050_REG_READ_TYPE readReg, MPU6050_REG_WRITE_TYPE writeReg, DELAY_MS_TYPE delay)
// {
//     //save old gyro full scale range
//     uint8_t gyroFs = 0;

//     readReg(REG_GYRO_CONFIG, &gyroFs);
    
//     gyroFs &= GYRO_FS_SEL_MASK; //keep only the FS_SEL setting

//     //set gyro to 250 dps for test
//     writeReg(REG_GYRO_CONFIG, GYRO_FS_SEL_250_DPS);

//     //wait
//     delay(250);

//     //get gyro's output with self test disabled
//     int16_t TD[3]; //3 axis
//     TD[0] = read_raw_axis(REG_GYRO_X_MEASURE_1, readReg);
//     TD[1] = read_raw_axis(REG_GYRO_Y_MEASURE_1, readReg);
//     TD[2] = read_raw_axis(REG_GYRO_Z_MEASURE_1, readReg);

//     //enable self test, and datasheet requires gyro set to 250 DPS
//     writeReg(
//         REG_GYRO_CONFIG, 
//         GYRO_FS_SEL_250_DPS | GYRO_XG_ST | GYRO_YG_ST | GYRO_ZG_ST
//     );

//     //wait
//     delay(250);
    
//     //get gyro's output with self test enabled
//     int16_t TE[3]; //3 axis
//     TE[0] = read_raw_axis(REG_GYRO_X_MEASURE_1, readReg);
//     TE[1] = read_raw_axis(REG_GYRO_Y_MEASURE_1, readReg);
//     TE[2] = read_raw_axis(REG_GYRO_Z_MEASURE_1, readReg);

//     //calculate the value of STR from the datasheet. This is
//     //different from reading the SELF_TEST (GTest) registers below
//     int16_t selfTestResponse[3];
//     selfTestResponse[0] = TE[0] - TD[0];
//     selfTestResponse[1] = TE[1] - TD[1];
//     selfTestResponse[2] = TE[2] - TD[2];
    

//     //read self test registers
//     uint8_t GTest[3];

//     readReg(REG_SELF_TEST_X, &GTest[0]);
//     readReg(REG_SELF_TEST_Y, &GTest[1]);
//     readReg(REG_SELF_TEST_Z, &GTest[2]);

//     GTest[0] &= XG_TEST_MASK;
//     GTest[1] &= YG_TEST_MASK;
//     GTest[2] &= ZG_TEST_MASK;
    
//     //calculate factory trims using self test registers
//     float factoryTrim[3];
//     factoryTrim[0] = 25.0f * 131.0f * powf(1.046f, (float)GTest[0] - 1.0f);
//     factoryTrim[1] = -25.0f * 131.0f * powf(1.046f, (float)GTest[1] - 1.0f); //y axis has -25.0 in datasheet
//     factoryTrim[2] = 25.0f * 131.0f * powf(1.046f, (float)GTest[2] - 1.0f);

//     //finally, calculate test results
//     FACTORY_TEST_RESULTS results;
//     results.failPercent = SELF_TEST_FAIL_PERCENT;
//     results.xAxis = 100.0f * (((float)selfTestResponse[0] - factoryTrim[0]) / factoryTrim[0]);
//     results.yAxis = 100.0f * (((float)selfTestResponse[1] - factoryTrim[1]) / factoryTrim[1]);
//     results.zAxis = 100.0f * (((float)selfTestResponse[2] - factoryTrim[2]) / factoryTrim[2]);

//     //revert test setup
//     writeReg(REG_GYRO_CONFIG, gyroFs);
    
//     return results;
// }

// FACTORY_TEST_RESULTS accel_self_test(MPU6050_REG_READ_TYPE readReg, MPU6050_REG_WRITE_TYPE writeReg, DELAY_MS_TYPE delay)
// {
//     //save old accel full scale range
//     uint8_t accelFs = 0;

//     readReg(REG_ACCEL_CONFIG, &accelFs);
    
//     accelFs &= ACCEL_FS_SEL_MASK; //keep only the FS_SEL setting

//     //set accel to 8g for test
//     writeReg(REG_ACCEL_CONFIG, ACCEL_FS_SEL_8G);

//     //wait
//     delay(250);

//     //get accels's output with self test disabled
//     int16_t TD[3]; //3 axis
//     TD[0] = read_raw_axis(REG_ACCEL_X_MEASURE_1, readReg);
//     TD[1] = read_raw_axis(REG_ACCEL_Y_MEASURE_1, readReg);
//     TD[2] = read_raw_axis(REG_ACCEL_Z_MEASURE_1, readReg);

//     //enable self test, and datasheet requires accel set to 8g
//     writeReg(
//         REG_ACCEL_CONFIG, 
//         ACCEL_FS_SEL_8G | ACCEL_XA_ST | ACCEL_YA_ST | ACCEL_ZA_ST
//     );

//     //wait
//     delay(250);
    
//     //get accels's output with self test enabled
//     int16_t TE[3]; //3 axis
//     TE[0] = read_raw_axis(REG_ACCEL_X_MEASURE_1, readReg);
//     TE[1] = read_raw_axis(REG_ACCEL_Y_MEASURE_1, readReg);
//     TE[2] = read_raw_axis(REG_ACCEL_Z_MEASURE_1, readReg);

//     //calculate the value of STR from the datasheet. This is
//     //different from reading the SELF_TEST (ATest) registers below
//     int16_t selfTestResponse[3];
//     selfTestResponse[0] = TE[0] - TD[0];
//     selfTestResponse[1] = TE[1] - TD[1];
//     selfTestResponse[2] = TE[2] - TD[2];
    

//     //read self test registers, each axis is 5 bits, split across two registers
//     uint8_t ATestUpper[3]; //more significant portion
//     uint8_t SELF_TEST_A; //less significant portion

//     //upper 3 bits
//     readReg(REG_SELF_TEST_X, &ATestUpper[0]);
//     readReg(REG_SELF_TEST_Y, &ATestUpper[1]);
//     readReg(REG_SELF_TEST_Z, &ATestUpper[2]);
    
//     //lower 2 bits
//     readReg(REG_SELF_TEST_A, &SELF_TEST_A);

//     //final combined accel test values
//     uint8_t ATest[3];
//     ATest[0] = ((ATestUpper[0] & XA_TEST_UPPER_MASK) >> 3) | ((SELF_TEST_A & XA_TEST_LOWER_MASK) >> 4);
//     ATest[1] = ((ATestUpper[1] & YA_TEST_UPPER_MASK) >> 3) | ((SELF_TEST_A & YA_TEST_LOWER_MASK) >> 2);
//     ATest[2] = ((ATestUpper[2] & ZA_TEST_UPPER_MASK) >> 3) | (SELF_TEST_A & ZA_TEST_LOWER_MASK);
    
//     //calculate factory trims using self test registers
//     float factoryTrim[3];
//     factoryTrim[0] = 4096.0f * 0.34f * powf((0.92f/0.34f), ((float)ATest[0] - 1) / 30.0f);
//     factoryTrim[1] = 4096.0f * 0.34f * powf((0.92f/0.34f), ((float)ATest[1] - 1) / 30.0f);
//     factoryTrim[2] = 4096.0f * 0.34f * powf((0.92f/0.34f), ((float)ATest[2] - 1) / 30.0f);

//     //finally, calculate test results
//     FACTORY_TEST_RESULTS results;
//     results.failPercent = SELF_TEST_FAIL_PERCENT;
//     results.xAxis = 100.0f * (((float)selfTestResponse[0] - factoryTrim[0]) / factoryTrim[0]);
//     results.yAxis = 100.0f * (((float)selfTestResponse[1] - factoryTrim[1]) / factoryTrim[1]);
//     results.zAxis = 100.0f * (((float)selfTestResponse[2] - factoryTrim[2]) / factoryTrim[2]);

//     //revert test setup
//     writeReg(REG_ACCEL_CONFIG, accelFs);
    
//     return results;
// }

uint16_t read_fifo_count(MPU6050_REG_READ_TYPE readReg)
{
    uint8_t upper = 0;
    uint8_t lower = 0;
    readReg(REG_FIFO_COUNT_H, &upper);
    readReg(REG_FIFO_COUNT_L, &lower);
    uint16_t combined = (((uint16_t)upper) << 8) | lower;
    return combined;
}
