ROOT_DIR ?= ..
SRC_DIRS ?= $(ROOT_DIR)/src $(ROOT_DIR)/sprites .
INCLUDE_DIRS ?= $(ROOT_DIR)/src $(ROOT_DIR)/sprites .

# Find all source files in the specified directories
SOURCES := $(shell find $(SRC_DIRS) -name '*.c' -or -name '*.S')
# Exclude labmain.c to avoid multiple definition of main
SOURCES := $(filter-out %/labmain.c, $(SOURCES))

OBJECTS := $(addsuffix .o, $(basename $(notdir $(SOURCES))))

# VPATH tells make where to look for source files
VPATH := $(SRC_DIRS)

LINKER ?= ./dtekv-script.lds

TOOLCHAIN ?= riscv32-unknown-elf-
CFLAGS ?= -Wall -nostdlib -O3 -mabi=ilp32 -march=rv32imzicsr -fno-builtin
CFLAGS += $(addprefix -I, $(INCLUDE_DIRS))


build: clean main.bin

main.elf: $(OBJECTS)
	$(TOOLCHAIN)ld -o $@ -T $(LINKER) $(filter-out boot.o, $(OBJECTS)) softfloat.a

# Rule to compile .c files
%.o: %.c
	$(TOOLCHAIN)gcc -c $(CFLAGS) $< -o $@

# Rule to compile .S files
%.o: %.S
	$(TOOLCHAIN)gcc -c $(CFLAGS) $< -o $@

main.bin: main.elf
	$(TOOLCHAIN)objcopy --output-target binary $< $@
	$(TOOLCHAIN)objdump -D $< > $<.txt

clean:
	rm -f *.o *.elf *.bin *.txt

TOOL_DIR ?= $(ROOT_DIR)/tools
run: main.bin
	make -C $(TOOL_DIR) "FILE_TO_RUN=$(CURDIR)/$<"
