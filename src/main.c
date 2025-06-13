#include <avr/io.h>
#include <avr/interrupt.h>

#include "activate_buzzer.h"
#include "bt_alert.h"
#include "motion_detect.h"
#include "rfid.h"
#include "sleep_check.h"

#include "buzzer.h"
#include "mfrc522_helper.h"
#include "mpu6050_helper.h"
#include "timer.h"
#include "sleep.h"
#include "uart.h"
#include "rtos.h"

#define LED_PIN PB5

int main(void) {
    DDRB |= (1 << LED_PIN);
    PORTB &= ~(1 << LED_PIN);

    init_timer1();

    uart_init(9600);

    rtos_init();

    init_buzz();
    init_rfid();
    init_mpu6050_w_interrupt();
    init_sleep();

    sei();

    rtos_add_task(check_rfid, 700, 0);
    rtos_add_task(check_sleep, 100, 100);
    rtos_add_task(detect_motion, 100, 0);
    rtos_add_task(buzzer_alert, 50, 10);
    rtos_add_task(send_bt_alert, 100, 50);


    while (1) {
        rtos_scheduler();
    }
}