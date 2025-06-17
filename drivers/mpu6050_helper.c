// Accelerometer module (MPU6050) helper/wrapper routines

#include "mpu6050_helper.h"


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

// Motion detection interrupt service routine
ISR(INT0_vect) {
    DEBUG_PRINT("INT0: Motion\n");
    set_motion_detected(true);
}

// Initialize motion interrupt pin on MPU6050
void setup_motion_interrupt(void) {
    cli();  // Disable interrupts during config

    // Configure INT0 (PD2, Arduino pin 2)
    EICRA |= (1 << ISC01) | (1 << ISC00);
    EIMSK |= (1 << INT0);                

    sei();  // Re-enable global interrupts
}

// MPU6050 initialization wrapper routine
int init_mpu6050_w_interrupt(void) {
    if (init_mpu6050(mpu6050_write_reg, rtos_delay_ms) != 0) {
        DEBUG_PRINT("MPU6050 initialization failed.\r\n");
        return -1;
    }

    setup_motion_interrupt();

    DEBUG_PRINT("MPU6050 initialization complete.\r\n");
    return 0;
}
