#ifndef MFRC522_HELPER_H
#define MFRC522_HELPER_H

#include "mfrc522.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "rtos.h"

#define UID_LENGTH 4

typedef struct {
    uint8_t uid[UID_LENGTH];
} rfid_tag_t;

// Global RFID instance
extern MFRC522 rfid;

void init_rfid(void);
rfid_tag_t mfrc522_get_tag(void);
bool is_authorized_tag(const rfid_tag_t* tag);

#endif // MFRC522_HELPER_H
