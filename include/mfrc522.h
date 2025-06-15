/* RFID scanner module (MFRC522) driver header file
* Adapted from https://github.com/miguelbalboa/rfid.git
* Arduino library for MFRC522 module by miguelbalboa
*/

#ifndef MFRC522_H
#define MFRC522_H

#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>
#include <string.h>
#include <avr/io.h>
#include <stdio.h>

#include "rtos.h"

#define MFRC522_UNUSED_PIN 0xFF

// --- Register addresses ---
#define CommandReg      0x01
#define ComIrqReg       0x04
#define FIFODataReg     0x09
#define FIFOLevelReg    0x0A
#define ControlReg      0x0C
#define BitFramingReg   0x0D
#define ErrorReg        0x06
#define Status2Reg      0x08
#define TxControlReg    0x14
#define ModeReg         0x11
#define TxModeReg       0x12
#define RxModeReg       0x13
#define TxASKReg        0x15
#define RFCfgReg        0x26
#define CollReg         0x0E
#define VersionReg      0x37
#define TModeReg        0x2A
#define TPrescalerReg   0x2B
#define TReloadRegH     0x2C
#define TReloadRegL     0x2D
#define ModWidthReg     0x24

// --- Commands ---
#define PCD_Idle            0x00
#define PCD_Mem             0x01
#define PCD_CalcCRC         0x03
#define PCD_Transceive      0x0C
#define PCD_SoftReset       0x0F

#define PICC_CMD_REQA       0x26
#define PICC_CMD_SEL_CL1    0x93

#define STATUS_OK           0
#define STATUS_ERROR        1
#define STATUS_COLLISION    2
#define STATUS_TIMEOUT      3
#define STATUS_NO_ROOM      4

typedef struct {
    uint8_t size;
    uint8_t uidByte[10];
    uint8_t sak;
} MFRC522_Uid;

typedef struct {
    uint8_t chipSelectPin; // (e.g., PB2)
    uint8_t resetPowerDownPin; // (e.g., PB1 or MFRC522_UNUSED_PIN)
} MFRC522;

void MFRC522_Init(MFRC522 *dev, uint8_t csPin, uint8_t rstPin);
void MFRC522_Reset(MFRC522 *dev);
void MFRC522_WriteRegister(MFRC522 *dev, uint8_t reg, uint8_t value);
uint8_t MFRC522_ReadRegister(MFRC522 *dev, uint8_t reg);
void MFRC522_SetRegisterBitMask(MFRC522 *dev, uint8_t reg, uint8_t mask);
void MFRC522_ClearRegisterBitMask(MFRC522 *dev, uint8_t reg, uint8_t mask);
void MFRC522_AntennaOn(MFRC522 *dev);
uint8_t MFRC522_CalculateCRC(MFRC522 *dev, uint8_t *data, uint8_t length, uint8_t *result);

bool MFRC522_PICC_IsNewCardPresent(MFRC522 *dev);
bool MFRC522_PICC_ReadCardSerial(MFRC522 *dev, MFRC522_Uid *uid);

#endif // MFRC522_H