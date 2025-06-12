#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "mfrc522.h"
#include "mfrc522_helper.h"
#include "rtos.h"

// int main(void) {
//     uart_init(9600);
//     init_rfid();  // Or whatever your RFID init function is called

//     DEBUG_PRINT("RFID test started\n");

//     while (1) {
//         rfid_tag_t tag = mfrc522_get_tag();

//         // Print UID if tag detected
//         int tag_present = 0;
//         for (int i = 0; i < UID_LENGTH; ++i) {
//             if (tag.uid[i] != 0) {
//                 tag_present = 1;
//                 break;
//             }
//         }

//         if (tag_present) {
//             DEBUG_PRINT("Tag UID: %02X %02X %02X %02X \n",
//                         tag.uid[0], tag.uid[1], tag.uid[2], tag.uid[3]);
//         } else {
//             DEBUG_PRINT("No tag present\n");
//         }

//         _delay_ms(500); // Poll every 0.5 seconds
//     }
// }

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

    init_sleep();
    init_buzz();
    init_rfid();
    init_mpu6050_w_interrupt();

    sei();

    rtos_add_task(detect_motion, 100, 0);
    rtos_add_task(check_rfid, 700, 35);
    rtos_add_task(buzzer_alert, 50, 10);
    rtos_add_task(send_bt_alert, 100, 50);
    rtos_add_task(check_sleep, 100, 100);


    while (1) {
        rtos_scheduler();
    }
}