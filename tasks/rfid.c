#include "mfrc522.h"
#include "controller_state.h"
#include "uart.h"
#include "rtos.h"

#define RFID_CS_PIN  PB2  // D10
#define RFID_RST_PIN PB1  // D9

MFRC522 rfid;

void check_rfid(void) {
    static bool initialized = false;
    //static MFRC522_Uid last_uid;

    if (!initialized) {
        MFRC522_Init(&rfid, RFID_CS_PIN, RFID_RST_PIN);
        initialized = true;
    }

    if (MFRC522_PICC_IsNewCardPresent(&rfid)) {
        MFRC522_Uid uid;
        if (MFRC522_PICC_ReadCardSerial(&rfid, &uid)) {
            // Optional: compare UID to authorized UID(s) here
            toggle_system_armed();

            printf("RFID tag scanned. System is now %s.\r\n", is_system_armed() ? "ARMED" : "DISARMED");
            delay_ms_rtos(1000); // debounce delay
        }
    }
}
