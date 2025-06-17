// Accelerometer (MPU6050) module helper functions header file

#ifndef MPU6050_HELPER_H
#define MPU6050_HELPER_H

#include <stdio.h>
#include <stddef.h>
#include <string.h> 
#include <stdint.h>
#include <avr/interrupt.h>

#include "mpu6050.h"
#include "reg_options.h"
#include "twi_master.h"
#include "controller_state.h"
#include "rtos.h"

int mpu6050_read_reg(uint8_t reg, uint8_t *data);
int mpu6050_write_reg(uint8_t reg, uint8_t data);
int init_mpu6050_w_interrupt(void);

#endif // MPU6050_HELPER_H
