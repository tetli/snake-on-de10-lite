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

unsigned int get_stall_count(void)
{
  unsigned int stalls;
  asm volatile("csrr %0, mhpmcounter3" : "=r"(stalls));
  return stalls;
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

  for (int i = 0; i < 1000; i++)
  {
    update_game_logic_only();
  }

  unsigned int end_cycles = get_cycles();
  unsigned int end_instret = get_instret();

  unsigned int total_cycles = end_cycles - start_cycles;
  unsigned int total_instret = end_instret - start_instret;

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

  // Re-initialize game for normal play
  game_init();

  // Timer setup
  set_timer(1000);
  TIMER_CONTROL = 0b0111;

  enable_interrupt();

  while (1)
    ;
}
