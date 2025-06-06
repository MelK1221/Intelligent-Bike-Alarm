# Makefile for AVR Timer1 LED Blink Test

MCU = atmega328p
F_CPU = 16000000UL
BAUD = 115200

TARGET = test_blink
SRC = $(TARGET).c
OBJ = $(SRC:.c=.o)

CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

CFLAGS = -std=c11 -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra
LDFLAGS = -mmcu=$(MCU)

# Update this to your Arduino serial port:
PORT = /dev/tty.usbmodem1101
PROGRAMMER = arduino

all: $(TARGET).hex

%.elf: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -D -U flash:w:$(TARGET).hex:i

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).hex
