#include "rfid_helper.h"

void init_rfid(void) {
    MFRC522_Init(&rfid, RFID_CS_PIN, RFID_RST_PIN);
}
