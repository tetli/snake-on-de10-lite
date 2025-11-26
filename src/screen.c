#include "screen.h"

#define VGA_MEMORY_BASE 0x08000000

volatile unsigned short *screen = (volatile unsigned short *)VGA_MEMORY_BASE;

/**
 * Fills the entire screen with a specific color.
 *
 * @param color The 16-bit color value to fill the screen with.
 */
void fill(unsigned short color)
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        *(screen + i) = color;
    }
}

/**
 * Draws a rectangle on the screen.
 *
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param color The 16-bit color value to fill the rectangle with.
 */
void draw_rect(int x, int y, int w, int h, unsigned short color)
{
    for (int row = y; row < y + h; row++)
    {
        for (int col = x; col < x + w; col++)
        {
            if (col >= 0 && col < SCREEN_WIDTH && row >= 0 && row < SCREEN_HEIGHT)
            {
                screen[row * SCREEN_WIDTH + col] = color;
            }
        }
    }
}

void handle_interrupt(void) {}

int main(void)
{
    fill(0x0000);
    draw_rect(0, 0, 10, 10, 0xFFFF);

    while (1)
        ;
    return 0;
}