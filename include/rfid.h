// RFID check task header file

#ifndef RFID_H
#define RFID_H

#include "mfrc522.h"
#include "mfrc522_helper.h"
#include "buzzer.h"
#include "controller_state.h"
#include "uart.h"
#include "rtos.h"
#include "sleep_check.h"

#ifdef UNIT_TEST
extern uint32_t last_scan_time;
#endif

void check_rfid(void);

#endif // RFID_H