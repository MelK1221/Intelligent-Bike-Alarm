// RFID scanner module (MFRC522) helper/wrapper routines

#include "mfrc522_helper.h"

#define NUM_AUTH_TAGS 2
#define RFID_RST_PIN PB1
#define RFID_CS_PIN PB2

MFRC522 rfid = {
    .chipSelectPin = RFID_CS_PIN,
    .resetPowerDownPin = RFID_RST_PIN
};

// Set authorized RFID Card UIDs
static const rfid_tag_t authorized_tags[] = {
    { .uid = {0x45, 0x88, 0x81, 0xAF} },
    { .uid = {0x63, 0x90, 0x0B, 0x32} }
};

// MFRC522 driver init routine wrapper
void init_rfid(void) {
    MFRC522_Init(&rfid, rfid.chipSelectPin, rfid.resetPowerDownPin);
    DEBUG_PRINT("MFRC522 Initialized\n");
}

// Check for RFID card presented and get UID if so
rfid_tag_t mfrc522_get_tag(void) {
    rfid_tag_t tag = {0};
    MFRC522_Uid uid = {0};
    bool temp1 = MFRC522_PICC_IsNewCardPresent(&rfid);
    if (temp1) {
        if (MFRC522_PICC_ReadCardSerial(&rfid, &uid)) {
            memcpy(tag.uid, uid.uidByte, UID_LENGTH);
            DEBUG_PRINT("RFID UID: %02X %02X %02X %02X\n", tag.uid[0], tag.uid[1], tag.uid[2], tag.uid[3]);
        } else {
            //
        }
    } else {
        DEBUG_PRINT("RFID: No tag present.\n");
    }
    return tag;
}

// Confirm if presented tag has an authorized UID
bool is_authorized_tag(const rfid_tag_t* tag) {
    for (int i = 0; i < NUM_AUTH_TAGS; ++i) {
        if (memcmp(tag->uid, authorized_tags[i].uid, UID_LENGTH) == 0) {
            return true;
        }
    }
    return false;
}