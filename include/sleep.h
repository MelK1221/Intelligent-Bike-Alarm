// Sleep mode driver header file

#ifndef SLEEP_H
#define SLEEP_H

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "sleep_check.h"
#include "mfrc522_helper.h"
#include "controller_state.h"
#include "rtos.h"

//extern volatile uint32_t last_wakeup_time;

void init_sleep(void);
void enter_sleep_mode(void);

#endif // SLEEP_H