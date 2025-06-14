// Main routine initializes RTOS and all modules, adds tasks to scheduler, and runs scheduler in continuous loop

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



int main(void) {
    // Set LED pin as output and turn off


    // Initialize system timer and comms
    init_timer1();
    uart_init(9600);

    // Initialize real time operating system
    rtos_init();

    // Initialize external alarm modules and Arduino sleep mode
    init_buzz();
    init_rfid();
    init_mpu6050_w_interrupt();
    init_sleep();

    // Enable interrupts
    sei();

    // Add tasks to scheduler
    rtos_add_task(detect_motion, 100, 0);
    rtos_add_task(buzzer_alert, 50, 25);
    rtos_add_task(send_bt_alert, 400, 230);
    rtos_add_task(check_rfid, 700, 375);
    rtos_add_task(check_sleep, 1000, 495);


    while (1) {
        // Run round-robin scheduler
        rtos_scheduler();
    }
}