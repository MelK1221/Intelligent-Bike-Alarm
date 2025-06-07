# === CONFIGURATION ===
MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = arduino
PORT = /dev/tty.usbmodem1101  # ← update this for your machine
BAUD = 115200

# === FILES ===
TARGET = bike_alarm
SRC = main.c rtos.c controller_state.c tasks/motion_detect.c tasks/bt_alert.c tasks/activate_buzzer.c drivers/uart.c drivers/mpu6050.c drivers/mpu6050_helper.c drivers/twi_master.c drivers/buzzer.c
OBJ = $(SRC:.c=.o)

# === TOOLS ===
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# === FLAGS ===
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall \
         -I. -Itasks -Idrivers
LDFLAGS = -mmcu=$(MCU) -Wl,-u,vfprintf -lprintf_flt -lm

# === DEFAULT TARGET ===
all: $(TARGET).hex

# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to ELF
$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

# Convert ELF to HEX
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Upload to Arduino
upload: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$(TARGET).hex:i

# Clean up build files
clean:
	rm -f *.o tasks/*.o drivers/*.o *.elf *.hex

.PHONY: all clean upload