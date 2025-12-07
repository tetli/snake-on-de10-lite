/*
    Written 2025 by Albin Tetli and Carl Malm
*/

#ifndef BOARD_H
#define BOARD_H

#define TIMER_BASE 0x04000020

#define TIMER_STATUS (*(volatile int *)(TIMER_BASE + 0x00))
#define TIMER_CONTROL (*(volatile int *)(TIMER_BASE + 0x04))
#define TIMER_PERIODL (*(volatile int *)(TIMER_BASE + 0x08))
#define TIMER_PERIODH (*(volatile int *)(TIMER_BASE + 0x0C))
#define TIMER_SNAPL (*(volatile int *)(TIMER_BASE + 0x10))
#define TIMER_SNAPH (*(volatile int *)(TIMER_BASE + 0x14))

void set_leds(int led_mask);
void set_displays(int display_number, int value);
void update_display_time(int time, int hourC);
int get_sw(void);
int get_btn(void);
void set_timer(int time);

#endif
