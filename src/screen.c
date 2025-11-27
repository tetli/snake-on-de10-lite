#include "screen.h"
#include "floorSprite.h"
#include "cubeSprite.h"

#define VGA_MEMORY_BASE 0x08000000

volatile unsigned char *screen = (volatile unsigned char *)VGA_MEMORY_BASE;

/**
 * Fills the entire screen with a specific color.
 *
 * @param color The 8-bit color value to fill the screen with.
 */
void fill(unsigned char color)
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
 * @param color The 8-bit color value to fill the rectangle with.
 */
void draw_rect(int x, int y, int w, int h, unsigned char color)
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

void draw_pixel(int x, int y, unsigned char color)
{
    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return;


    screen[y * SCREEN_WIDTH + x] = color; // unsigned char is 8bit, same as the display
}

void draw_tile(int x, int y, unsigned char borderColor, unsigned char innerColor){
    for (int row = 0; row < FLOOR_FRAME_HEIGHT; row++) {
        for (int col = 0; col < FLOOR_FRAME_WIDTH; col++) {

            int colorType = floorTileSprite[row][col];
            if (colorType == 0) continue; // 0 = empty

            int screenX = x + col;
            int screenY = y + row;

            if (screenX >= 0 && screenX < SCREEN_WIDTH &&
                screenY >= 0 && screenY < SCREEN_HEIGHT) {
                if (colorType == 1) draw_pixel(screenX, screenY, borderColor);
                else if (colorType == 2) draw_pixel(screenX, screenY, innerColor);
            }
        }
    } 
}

void draw_cube(int x, int y, unsigned char borderColor, unsigned char innerColor){
    for (int row = 0; row < CUBE_FRAME_HEIGHT; row++) {
        for (int col = 0; col < CUBE_FRAME_WIDTH; col++) {

            int colorType = cubeSprite[row][col];
            if (colorType == 0) continue; // 0 = empty

            int screenX = x + col;
            int screenY = y + row;

            if (screenX >= 0 && screenX < SCREEN_WIDTH &&
                screenY >= 0 && screenY < SCREEN_HEIGHT) {
                if (colorType == 1) draw_pixel(screenX, screenY, borderColor);
                else if (colorType == 2) draw_pixel(screenX, screenY, innerColor);
            }
        }
    }
}
