#include "mfrc522.h"

#include <util/delay.h>
#include <string.h>
#include <avr/io.h>

// Adjust these to your wiring
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_PIN  PINB

#define SPI_MOSI PB3
#define SPI_MISO PB4
#define SPI_SCK  PB5

#define CRCResultRegL 0x22
#define CRCResultRegH 0x21
#define DivIrqReg     0x05

// --- SPI helper ---

static void SPI_init(void) {
    SPI_DDR |= (1<<SPI_MOSI)|(1<<SPI_SCK);
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

static uint8_t SPI_transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1<<SPIF)));
    return SPDR;
}

static void MFRC522_select(MFRC522 *dev) {
    SPI_PORT &= ~(1 << dev->chipSelectPin);
}

static void MFRC522_unselect(MFRC522 *dev) {
    SPI_PORT |= (1 << dev->chipSelectPin);
}

// --- Register R/W ---

void MFRC522_WriteRegister(MFRC522 *dev, uint8_t reg, uint8_t value) {
    MFRC522_select(dev);
    SPI_transfer((reg << 1) & 0x7E);
    SPI_transfer(value);
    MFRC522_unselect(dev);
}

uint8_t MFRC522_ReadRegister(MFRC522 *dev, uint8_t reg) {
    uint8_t value;
    MFRC522_select(dev);
    SPI_transfer(((reg << 1) & 0x7E) | 0x80);
    value = SPI_transfer(0x00);
    MFRC522_unselect(dev);
    return value;
}

void MFRC522_SetRegisterBitMask(MFRC522 *dev, uint8_t reg, uint8_t mask) {
    uint8_t tmp = MFRC522_ReadRegister(dev, reg);
    MFRC522_WriteRegister(dev, reg, tmp | mask);
}

void MFRC522_ClearRegisterBitMask(MFRC522 *dev, uint8_t reg, uint8_t mask) {
    uint8_t tmp = MFRC522_ReadRegister(dev, reg);
    MFRC522_WriteRegister(dev, reg, tmp & (~mask));
}

// --- Core functions ---

void MFRC522_Init(MFRC522 *dev, uint8_t csPin, uint8_t rstPin) {
    dev->chipSelectPin = csPin;
    dev->resetPowerDownPin = rstPin;

    SPI_DDR |= (1 << csPin);
    SPI_PORT |= (1 << csPin); // unselect

    if (rstPin != MFRC522_UNUSED_PIN) {
        SPI_DDR |= (1 << rstPin);
        SPI_PORT |= (1 << rstPin);
    }

    SPI_init();

    // Optional hardware reset
    if (rstPin != MFRC522_UNUSED_PIN) {
        SPI_PORT &= ~(1 << rstPin);
        _delay_us(2);
        SPI_PORT |= (1 << rstPin);
        _delay_ms(50);
    }

    MFRC522_Reset(dev);

    MFRC522_WriteRegister(dev, TModeReg, 0x8D);
    MFRC522_WriteRegister(dev, TPrescalerReg, 0x3E);
    MFRC522_WriteRegister(dev, TReloadRegL, 30);
    MFRC522_WriteRegister(dev, TReloadRegH, 0);
    MFRC522_WriteRegister(dev, TxASKReg, 0x40);
    MFRC522_WriteRegister(dev, ModeReg, 0x3D);
    MFRC522_AntennaOn(dev);
}

void MFRC522_Reset(MFRC522 *dev) {
    MFRC522_WriteRegister(dev, CommandReg, PCD_SoftReset);
    _delay_ms(50);
}

void MFRC522_AntennaOn(MFRC522 *dev) {
    uint8_t value = MFRC522_ReadRegister(dev, TxControlReg);
    if ((value & 0x03) != 0x03) {
        MFRC522_WriteRegister(dev, TxControlReg, value | 0x03);
    }
}

uint8_t MFRC522_CalculateCRC(MFRC522 *dev, uint8_t *data, uint8_t length, uint8_t *result) {
    MFRC522_WriteRegister(dev, CommandReg, PCD_Idle);
    MFRC522_WriteRegister(dev, DivIrqReg, 0x04); // Clear CRCIRq
    MFRC522_WriteRegister(dev, FIFOLevelReg, 0x80); // Flush FIFO

    // Write data to FIFO
    for (uint8_t i = 0; i < length; i++)
        MFRC522_WriteRegister(dev, FIFODataReg, data[i]);

    MFRC522_WriteRegister(dev, CommandReg, PCD_CalcCRC);

    // Wait for CRC completion or timeout (~89ms)
    uint16_t i;
    for (i = 0; i < 5000; i++) {
        uint8_t n = MFRC522_ReadRegister(dev, DivIrqReg);
        if (n & 0x04) break; // CRCIRq bit set
        _delay_us(20);
    }
    if (i == 5000) return STATUS_TIMEOUT; // Timed out

    // Read CRC result
    result[0] = MFRC522_ReadRegister(dev, CRCResultRegL);
    result[1] = MFRC522_ReadRegister(dev, CRCResultRegH);

    return STATUS_OK;
}

// --- Minimal Transceive, RequestA, Select, UID read ---

static uint8_t MFRC522_TransceiveData(
    MFRC522 *dev,
    uint8_t *sendData, uint8_t sendLen,
    uint8_t *backData, uint8_t *backLen,
    uint8_t *validBits, uint8_t rxAlign
) {
    uint8_t waitIRq = 0x30; // RxIRq, IdleIRq
    uint8_t n, _validBits = 0;
    uint16_t i;
    *backLen = 0;

    MFRC522_WriteRegister(dev, CommandReg, PCD_Idle);
    MFRC522_WriteRegister(dev, ComIrqReg, 0x7F);
    MFRC522_WriteRegister(dev, FIFOLevelReg, 0x80);

    for (i = 0; i < sendLen; i++)
        MFRC522_WriteRegister(dev, FIFODataReg, sendData[i]);
    MFRC522_WriteRegister(dev, BitFramingReg, (rxAlign << 4) + (validBits ? *validBits : 0));
    MFRC522_WriteRegister(dev, CommandReg, PCD_Transceive);
    MFRC522_SetRegisterBitMask(dev, BitFramingReg, 0x80);

    // Wait for the command to complete (timeout ~25ms)
    for (i = 0; i < 1000; i++) {
        n = MFRC522_ReadRegister(dev, ComIrqReg);
        if (n & waitIRq) break;
        if (n & 0x01) return STATUS_TIMEOUT;
        _delay_us(25);
    }
    if (i == 1000) return STATUS_TIMEOUT;

    // Check for errors
    n = MFRC522_ReadRegister(dev, ErrorReg);
    if (n & 0x13) return STATUS_ERROR;

    // Read received data
    n = MFRC522_ReadRegister(dev, FIFOLevelReg);
    if (n > 0 && backData && backLen) {
        *backLen = n;
        for (i = 0; i < n; i++) backData[i] = MFRC522_ReadRegister(dev, FIFODataReg);
        _validBits = MFRC522_ReadRegister(dev, ControlReg) & 0x07;
        if (validBits) *validBits = _validBits;
    }

    return STATUS_OK;
}

static uint8_t MFRC522_RequestA(MFRC522 *dev, uint8_t *bufferATQA, uint8_t *bufferSize) {
    uint8_t command = PICC_CMD_REQA;
    uint8_t validBits = 7;
    *bufferSize = 2;
    return MFRC522_TransceiveData(dev, &command, 1, bufferATQA, bufferSize, &validBits, 0);
}

bool MFRC522_PICC_IsNewCardPresent(MFRC522 *dev) {
    uint8_t bufferATQA[2], bufferSize = 2;
    return (MFRC522_RequestA(dev, bufferATQA, &bufferSize) == STATUS_OK);
}

bool MFRC522_PICC_ReadCardSerial(MFRC522 *dev, MFRC522_Uid *uid) {
    uint8_t buffer[9], bufferUsed = 0, responseLen = 3, txLastBits = 0, rxAlign = 0;
    uint8_t status;
    // Anticollision command (cascade level 1)
    buffer[0] = PICC_CMD_SEL_CL1;
    buffer[1] = 0x20;
    bufferUsed = 2;
    status = MFRC522_TransceiveData(dev, buffer, bufferUsed, &buffer[2], &bufferUsed, &txLastBits, rxAlign);
    if (status != STATUS_OK) return false;
    memcpy(uid->uidByte, &buffer[2], 4);
    uid->size = 4;
    // SELECT command
    buffer[0] = PICC_CMD_SEL_CL1;
    buffer[1] = 0x70;
    memcpy(&buffer[2], uid->uidByte, 4);
    buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5]; // BCC
    if (MFRC522_CalculateCRC(dev, buffer, 7, &buffer[7]) != STATUS_OK)
        return false;
    bufferUsed = 9;
    responseLen = 3; txLastBits = 0; rxAlign = 0;
    status = MFRC522_TransceiveData(dev, buffer, bufferUsed, &buffer[2], &responseLen, &txLastBits, rxAlign);
    if (status != STATUS_OK) return false;
    uid->sak = buffer[2];
    return true;
}