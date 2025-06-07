#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rtos.h"
#include "uart.h"
#include "mpu6050.h"
#include "mpu6050_helper.h"
#include "buzzer.h"

// RTOS task: Blink LED at PB5 roughly every second
void task_blink(void) {
    static uint16_t count = 0;
    static uint8_t led_state = 0;

    if (++count >= 100) {  // Adjust to your timer tick (e.g., 10ms * 100 = 1s)
        if (led_state) {
            PORTB |= (1 << PB5);  // LED ON
        } else {
            PORTB &= ~(1 << PB5); // LED OFF
        }
        led_state = !led_state;
        count = 0;
    }
}

extern void detect_motion(void);
extern void send_bt_alert(void);
extern void buzzer_alert(void);
extern void check_rfid(void);

void timer1_init_10ms(void) {
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    OCR1A = 2499;  // For 10ms @ 16MHz / 64 prescaler
    TCCR1B |= (1 << WGM12);             // CTC mode
    TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler 64
    TIMSK1 |= (1 << OCIE1A);            // Enable compare interrupt
}

ISR(TIMER1_COMPA_vect) {
    rtos_clock();
}

int main(void) {
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    timer1_init_10ms();
    sei();

    uart_init(9600);
    buzz_init();

    rtos_init();

    if (init_mpu6050(mpu6050_write_reg, delay_ms_rtos) != 0) {
        printf("MPU6050 initialization failed.\r\n");
        while (1);  // Stop if init fails
    }
    calibrate_mpu6050(200);
    calibrate_gyro();

    rtos_add_task(detect_motion, 5, 0);
    // rtos_add_task(task_blink, 1, 0);
    rtos_add_task(send_bt_alert, 100, 0);
    rtos_add_task(buzzer_alert, 5, 2);
    rtos_add_task(check_rfid, 100, 200);

    while (1) {
        rtos_scheduler();
    }
}

