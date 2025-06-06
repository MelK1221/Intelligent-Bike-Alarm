#ifndef MPU6050_HELPER_H
#define MPU6050_HELPER_H

#include <stdint.h>

int mpu6050_read_reg(uint8_t reg, uint8_t *data);
int mpu6050_write_reg(uint8_t reg, uint8_t data);
int mpu6050_read_burst(uint16_t reg, uint8_t *data, uint16_t len);
void delay_ms(uint32_t ms);
void mpu6050_read_raw(int16_t *ax, int16_t *ay, int16_t *az);

#endif
