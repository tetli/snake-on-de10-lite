/*
    Modified 2025 by Albin Tetli and Carl Malm
*/

BUILD_DIR := build
SRC_DIR := src
SYSTEM_DIR := system
SPRITES_DIR := sprites

# Toolchain and flags
TOOLCHAIN ?= riscv32-unknown-elf-
CC := $(TOOLCHAIN)gcc
LD := $(TOOLCHAIN)ld
OBJCOPY := $(TOOLCHAIN)objcopy
OBJDUMP := $(TOOLCHAIN)objdump

CFLAGS ?= -Wall -nostdlib -O3 -mabi=ilp32 -march=rv32imzicsr -fno-builtin
LDFLAGS ?= --no-warn-rwx-segments
INCLUDES := -I$(SRC_DIR) -I$(SYSTEM_DIR) -I$(SPRITES_DIR)

# Linker script and libraries
LINKER_SCRIPT := $(SYSTEM_DIR)/dtekv-script.lds

# Find sources
SOURCES_SRC := $(shell find $(SRC_DIR) -name '*.c')
SOURCES_SYSTEM := $(shell find $(SYSTEM_DIR) -name '*.c' -or -name '*.S')
SOURCES_SPRITES := $(shell find $(SPRITES_DIR) -name '*.c')

SOURCES := $(SOURCES_SRC) $(SOURCES_SYSTEM) $(SOURCES_SPRITES)

# Define objects
OBJECTS := $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

# VPATH
vpath %.c $(SRC_DIR) $(SYSTEM_DIR) $(SPRITES_DIR)
vpath %.S $(SRC_DIR) $(SYSTEM_DIR) $(SPRITES_DIR)

.PHONY: all clean

all: $(BUILD_DIR)/main.bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(BUILD_DIR)/%.o: %.S | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(BUILD_DIR)/main.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -L $(BUILD_DIR) -o $@ -T $(LINKER_SCRIPT) $(filter-out $(BUILD_DIR)/boot.o, $(OBJECTS))

$(BUILD_DIR)/main.bin: $(BUILD_DIR)/main.elf
	$(OBJCOPY) --output-target binary $< $@
	$(OBJDUMP) -D $< > $<.txt

clean:
	rm -rf $(BUILD_DIR)
