#include "mfrc522_helper.h"

#include <string.h>

#define NUM_AUTH_TAGS 2

static const rfid_tag_t authorized_tags[] = {
    { .uid = {0x45, 0x88, 0x81, 0xAF} },
    { .uid = {0x63, 0x90, 0x0B, 0x32} }
};

void init_rfid(void) {
    MFRC522_Init(&rfid, RFID_CS_PIN, RFID_RST_PIN);
}

rfid_tag_t mfrc522_get_tag(void) {
    rfid_tag_t tag;
    mfrc522_get_uid(tag.uid);  // assumes 4-byte UID is filled here
    return tag;
}

bool is_authorized_tag(const rfid_tag_t* tag) {
    for (int i = 0; i < NUM_AUTH_TAGS; ++i) {
        if (memcmp(tag->uid, authorized_tags[i].uid, UID_LENGTH) == 0) {
            return true;
        }
    }
    return false;
}