# Compiler and flags
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-mmcu=atmega328p -Os -Wall

# Source object files
OBJ=main.o rtos/rtos.o tasks/task_motion.o tasks/task_rfid.o tasks/task_buzzer.o tasks/task_alert_bt.o \
    drivers/mpu6050.o drivers/mfrc522.o drivers/buzzer.o drivers/bluetooth.o utils/timer.o

# Targets
TARGET=SmartBikeAlarm
MCU=atmega328p
PORT=/dev/cu.usbmodem1101
BAUD=115200

# Default target
all: $(TARGET).hex

# Compile and link
$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Convert ELF to HEX
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Upload using avrdude
upload: $(TARGET).hex
	avrdude -c arduino -p $(MCU) -P $(PORT) -b $(BAUD) -D -U flash:w:$(TARGET).hex:i

# Clean build files
clean:
	rm -f *.o */*.o *.elf *.hex