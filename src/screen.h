#ifndef SCREEN_H
#define SCREEN_H

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

/**
 * Fills the entire screen with a specific color.
 *
 * @param color The 16-bit color value to fill the screen with.
 */
void fill(unsigned char color);

/**
 * Draws a rectangle on the screen.
 *
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param color The 16-bit color value to fill the rectangle with.
 */
void draw_rect(int x, int y, int w, int h, unsigned char color);

/**
 * Draws a full screen image.
 *
 * @param image_data Pointer to the image data array (must be SCREEN_WIDTH * SCREEN_HEIGHT bytes).
 */
void draw_image(const unsigned char *image_data);

#endif // SCREEN_H
