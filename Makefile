# Project name
TARGET = smart-bike-alarm

# MCU and programmer settings
MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = arduino
PORT = /dev/ttyUSB0
BAUD = 115200

# Toolchain
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# Directories
SRC_DIRS = src drivers tasks
INC_DIRS = include
BUILD_DIR = build

# File extensions
SRC_EXT = c
OBJ_EXT = o

# Compiler flags
CFLAGS = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CFLAGS += $(addprefix -I,$(INC_DIRS))  # Include header directories
CFLAGS += -DDEBUG_RTOS

# Find all source files
SRCS := $(shell find $(SRC_DIRS) -name '*.$(SRC_EXT)')
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

# Default rule
all: $(BUILD_DIR)/$(TARGET).hex

# Compile each object file
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into ELF binary
$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Convert ELF to HEX
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Upload using avrdude
upload: $(BUILD_DIR)/$(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$<

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all upload clean
