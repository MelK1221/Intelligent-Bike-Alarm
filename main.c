#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>
#include <util/delay.h>
#include "rtos.h"
#include "uart.h"
#include "mpu6050.h"
#include "mpu6050_helper.h"

// Test RTOS task
void task_blink(void) {
    static uint16_t count = 0;

    if (++count >= 100) {  // Adjust to ~1 second
        static uint8_t led_state = 0;

        if (led_state) {
            PORTB |= (1 << PB5);  // LED ON
            printf("LED ON\n");
        } else {
            PORTB &= ~(1 << PB5);   // LED OFF
            printf("LED OFF\n");

            // Delay while OFF to make it visible
            _delay_ms(500);  // Keep LED off for 0.5s
        }

        led_state = !led_state;
        count = 0;
    }
}


extern void detect_motion(void);
// extern void calibrate_motion(void);

void timer1_init_10ms(void) {
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;    
    
    OCR1A = 2499;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS11) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
    rtos_clock();
}

int main(void) {
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    timer1_init_10ms();
    sei();
    rtos_init();
    uart_init(115200);
    printf("UART Initialize!\r\n");
    fflush(stdout);
    _delay_ms(100);

    printf("Before MPU6050 init\r\n");
    fflush(stdout);
    uint16_t result = init_mpu6050(mpu6050_write_reg, delay_ms);
    if (result != 0) {
        printf("MPU6050 init failed with code %d\n", result);
    } else {
        printf("MPU6050 initialized successfully\n");
    }


    //rtos_add_task(calibrate_motion, 5, 0);
    rtos_add_task(task_blink, 1, 0);
    // printf("Tasks added: %u\n", task_count);


    // set_sleep_mode(SLEEP_MODE_IDLE);
    // sleep_enable();
    

    while (1) {
        rtos_scheduler();
        //sleep_cpu();

        // static uint32_t loop_count = 0;
        // if (loop_count++ % 1000 == 0) {
        //     printf("Main loop running\n");
        //     fflush(stdout);
        // }
    }
}

