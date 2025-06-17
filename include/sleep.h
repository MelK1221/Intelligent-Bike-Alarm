// Sleep mode driver header file

#ifndef SLEEP_H
#define SLEEP_H

#ifdef __AVR__
#include <avr/sleep.h>
#include <avr/interrupt.h>
#endif

#include <stdint.h>

#include "sleep_check.h"
#include "mfrc522_helper.h"
#include "controller_state.h"
#include "rtos.h"

void init_sleep(void);
void enter_sleep_mode(void);

#endif // SLEEP_H