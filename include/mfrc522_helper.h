#ifndef RFID_HELPER_H
#define RFID_HELPER_H

#include "mfrc522.h"

#include <stdint.h>
#include <stdbool.h>

#define UID_LENGTH 4

typedef struct {
    uint8_t uid[UID_LENGTH];
} rfid_tag_t;

// Global RFID instance
extern MFRC522 rfid;

void init_rfid(void);
rfid_tag_t mfrc522_get_tag(void);
bool is_authorized_tag(const rfid_tag_t* tag);

#endif // RFID_HELPER_H
