/*
    Written 2025 by Albin Tetli and Carl Malm
*/

#include "board.h"

void set_leds(int led_mask)
{
    volatile int *pLedA = (volatile int *)0x04000000;

    int least10bitsLed = led_mask & 0x3FF;

    *pLedA = least10bitsLed;
}

void set_displays(int display_number, int value)
{
    volatile int *pDisplayA = (volatile int *)(0x04000050 + (display_number * 0x10));

    int number = 0;

    switch (value)
    {
    case 0:
        number = 0b11000000;
        break;
    case 1:
        number = 0b11111001;
        break;
    case 2:
        number = 0b10100100;
        break;
    case 3:
        number = 0b10110000;
        break;
    case 4:
        number = 0b10011001;
        break;
    case 5:
        number = 0b10010010;
        break;
    case 6:
        number = 0b10000010;
        break;
    case 7:
        number = 0b11111000;
        break;
    case 8:
        number = 0b10000000;
        break;
    case 9:
        number = 0b10010000;
        break;

    default:
        break;
    }

    *pDisplayA = number;
}

void update_display_time(int time, int hourC)
{
    set_displays(0, time & 0xF);
    set_displays(1, (time & 0xF0) >> 4);
    set_displays(2, (time & 0xF00) >> 8);
    set_displays(3, (time & 0xF000) >> 12);
    set_displays(4, hourC % 10);
    set_displays(5, hourC / 10);
}

int get_sw(void)
{
    volatile int *pSWA = (volatile int *)0x04000010;

    int least10bitsSW = *pSWA & 0x3FF;

    return least10bitsSW;
}

int get_btn(void)
{
    volatile int *pBtnA = (volatile int *)0x040000d0;

    int lsbBtn = *pBtnA & 0x1;

    return lsbBtn;
}

/* Set timer to input (milliseconds)*/
void set_timer(int time)
{
    unsigned int time_val = (unsigned int)time * 30000U; // // Konvertera time till rätt format. 30MHz
    unsigned int timeLower = time_val & 0xFFFF;
    unsigned int timeUpper = (time_val >> 16) & 0xFFFF;
    TIMER_PERIODL = timeLower;
    TIMER_PERIODH = timeUpper;
}
