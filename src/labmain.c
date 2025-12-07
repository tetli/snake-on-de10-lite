/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */

/* Below functions are external and found in other files. */

#include "board.h"
#include "game.h"
#include "../system/dtekv-lib.h"

extern void enable_interrupt(void);

unsigned int get_cycles(void)
{
  unsigned int cycles;
  asm volatile("csrr %0, mcycle" : "=r"(cycles));
  return cycles;
}

unsigned int get_instret(void)
{
  unsigned int instret;
  asm volatile("csrr %0, minstret" : "=r"(instret));
  return instret;
}

unsigned int get_hpm_counter(int n)
{
  unsigned int val = 0;
  switch (n)
  {
  case 3:
    asm volatile("csrr %0, mhpmcounter3" : "=r"(val));
    break;
  case 4:
    asm volatile("csrr %0, mhpmcounter4" : "=r"(val));
    break;
  case 5:
    asm volatile("csrr %0, mhpmcounter5" : "=r"(val));
    break;
  case 6:
    asm volatile("csrr %0, mhpmcounter6" : "=r"(val));
    break;
  case 7:
    asm volatile("csrr %0, mhpmcounter7" : "=r"(val));
    break;
  case 8:
    asm volatile("csrr %0, mhpmcounter8" : "=r"(val));
    break;
  case 9:
    asm volatile("csrr %0, mhpmcounter9" : "=r"(val));
    break;
  default:
    break;
  }
  return val;
}

int mytime = 0x5957;
int timeoutcount = 0;
volatile int previousTime = 0;
volatile int hourCount = 0;

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause)
{
  if (cause == 16)
  {
    TIMER_STATUS = 0x0;
    timeoutcount++;
    // update_time();

    game_tick();
  }
}

int main(void)
{
  // Initialize game state for performance measurement
  game_init();

  // Performance measurement
  unsigned int start_cycles = get_cycles();
  unsigned int start_instret = get_instret();
  unsigned int start_hpm3 = get_hpm_counter(3);
  unsigned int start_hpm4 = get_hpm_counter(4);
  unsigned int start_hpm5 = get_hpm_counter(5);
  unsigned int start_hpm6 = get_hpm_counter(6);
  unsigned int start_hpm7 = get_hpm_counter(7);
  unsigned int start_hpm8 = get_hpm_counter(8);
  unsigned int start_hpm9 = get_hpm_counter(9);

  for (int i = 0; i < 1000; i++)
  {
    update_game_logic_only();
  }

  unsigned int end_cycles = get_cycles();
  unsigned int end_instret = get_instret();
  unsigned int end_hpm3 = get_hpm_counter(3);
  unsigned int end_hpm4 = get_hpm_counter(4);
  unsigned int end_hpm5 = get_hpm_counter(5);
  unsigned int end_hpm6 = get_hpm_counter(6);
  unsigned int end_hpm7 = get_hpm_counter(7);
  unsigned int end_hpm8 = get_hpm_counter(8);
  unsigned int end_hpm9 = get_hpm_counter(9);

  unsigned int total_cycles = end_cycles - start_cycles;
  unsigned int total_instret = end_instret - start_instret;
  unsigned int total_hpm3 = end_hpm3 - start_hpm3;
  unsigned int total_hpm4 = end_hpm4 - start_hpm4;
  unsigned int total_hpm5 = end_hpm5 - start_hpm5;
  unsigned int total_hpm6 = end_hpm6 - start_hpm6;
  unsigned int total_hpm7 = end_hpm7 - start_hpm7;
  unsigned int total_hpm8 = end_hpm8 - start_hpm8;
  unsigned int total_hpm9 = end_hpm9 - start_hpm9;

  print("Total Cycles: ");
  print_dec(total_cycles);
  print("\n");
  print("Total Instructions: ");
  print_dec(total_instret);
  print("\n");
  print("IPC: ");
  if (total_cycles > 0)
  {
    unsigned int ipc_int = total_instret / total_cycles;
    unsigned int ipc_frac = ((total_instret % total_cycles) * 100) / total_cycles;
    print_dec(ipc_int);
    print(".");
    if (ipc_frac < 10)
      print("0");
    print_dec(ipc_frac);
  }
  else
  {
    print("N/A");
  }
  print("\n");

  print("Memory Instructions (hpm3): ");
  print_dec(total_hpm3);
  print("\n");

  print("I-Cache Misses (hpm4): ");
  print_dec(total_hpm4);
  print("\n");

  print("D-Cache Misses (hpm5): ");
  print_dec(total_hpm5);
  print("\n");

  print("I-Cache Stalls (hpm6): ");
  print_dec(total_hpm6);
  print("\n");

  print("D-Cache Stalls (hpm7): ");
  print_dec(total_hpm7);
  print("\n");

  print("Data Hazard Stalls (hpm8): ");
  print_dec(total_hpm8);
  print("\n");

  print("ALU Stalls (hpm9): ");
  print_dec(total_hpm9);
  print("\n");

  // Re-initialize game for normal play
  game_init();

  // Timer setup
  set_timer(1000);
  TIMER_CONTROL = 0b0111;

  enable_interrupt();

  while (1)
    ;
}
