#include <avr/io.h>
#include <avr/interrupt.h>

#include "buzzer.h"
#include "mpu6050.h"
#include "mpu6050_helper.h"
#include "timer.h"
#include "uart.h"
#include "rtos.h"

#define LED_PIN PB5

extern void detect_motion(void);
extern void send_bt_alert(void);
extern void buzzer_alert(void);
extern void check_rfid(void);

int main(void) {
    DDRB |= (1 << LED_PIN);
    PORTB &= ~(1 << LED_PIN);

    init_timer1();
    sei();

    init_uart(9600);
    init_buzzer();
    init_rfid();
    rtos_init();

    if (init_mpu6050(mpu6050_write_reg, rtos_delay_ms) != 0) {
        printf("MPU6050 initialization failed.\r\n");
        while (1);  // Stop if init fails
    }
    calibrate_mpu6050(200);
    calibrate_gyro();

    rtos_add_task(detect_motion, 5, 0);
    rtos_add_task(send_bt_alert, 100, 0);
    rtos_add_task(buzzer_alert, 5, 2);
    rtos_add_task(check_rfid, 100, 200);

    while (1) {
        rtos_scheduler();
    }
}