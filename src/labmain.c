/*
    Written 2025 by Albin Tetli and Carl Malm
*/

#include "board.h"
#include "game.h"

extern void enable_interrupt(void);

int mytime = 0x5957;
int timeoutcount = 0;
volatile int previousTime = 0;
volatile int hourCount = 0;

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
  game_init();

  // Timer setup
  set_timer(1000);
  TIMER_CONTROL = 0b0111;

  enable_interrupt();

  while (1)
    ;
}
