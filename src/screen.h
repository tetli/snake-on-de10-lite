#ifndef SCREEN_H
#define SCREEN_H

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

/**
 * Fills the entire screen with a specific color.
 *
 * @param color The 16-bit color value to fill the screen with.
 */
void fill(unsigned short color);

/**
 * Draws a rectangle on the screen.
 *
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param color The 16-bit color value to fill the rectangle with.
 */
void draw_rect(int x, int y, int w, int h, unsigned short color);

#endif // SCREEN_H
