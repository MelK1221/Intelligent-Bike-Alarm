#ifndef RFID_HELPER_H
#define RFID_HELPER_H

#include "mfrc522.h"

// Global RFID instance
extern MFRC522 rfid;

void init_rfid(void);

#endif
