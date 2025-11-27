# Snake on DE10-Lite

A classic Snake game implementation for the DE10-Lite FPGA board, running on a RISC-V soft-core processor.

## Overview

This project implements the Snake game using C and Assembly. It is designed to run on a bare-metal RISC-V environment (specifically targeting a soft-core on the DE10-Lite). The game features graphical output, interrupt-driven game logic, and hardware control via memory-mapped I/O.

## Features

- **Classic Gameplay**: Navigate the snake to eat food and grow, avoiding walls and your own tail.
- **Graphics**: 320x240 resolution rendering.
- **Hardware Integration**:
  - **Timer Interrupts**: Controls the game tick rate for movement.
  - **Input**: Uses onboard buttons/switches for control.

## Project Structure

- `src/`: Source code for the game logic (`game.c`), hardware abstraction (`board.c`), and screen rendering (`screen.c`).
- `system/`: System-level code including bootloader (`boot.S`), linker script (`dtekv-script.lds`), and library functions.
- `sprites/`: C files containing sprite data.
- `build/`: Output directory for compiled binaries.
- `Makefile`: Build configuration.

## Prerequisites

To build this project, you need a RISC-V toolchain installed:

- `riscv32-unknown-elf-gcc`
- `riscv32-unknown-elf-ld`
- `riscv32-unknown-elf-objcopy`
- `riscv32-unknown-elf-objdump`

## Building

1. Clone the repository.
2. Run `make` in the root directory:

   ```bash
   make
   ```

3. The build artifacts will be generated in the `build/` directory:
   - `main.elf`: The linked executable.
   - `main.bin`: Binary file ready for upload.
   - `main.elf.txt`: Disassembly for debugging.

## Controls

(Note: Specific mappings depend on `board.c` implementation)

- **Movement**: Onboard Buttons / Switches.
- **Reset/Start**: Specific button or switch combination.
