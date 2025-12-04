#include "game.h"
#include "board.h"
#include "screen.h"
#include "start.h"
#include "end.h"
#include "floorSprite.h"
#include "cubeSprite.h"
/* Game variables */
static int gridmap[MAP_XWIDTH][MAP_YHEIGHT] = {0};
int previousDrawMode = 0;
int isoDrawMode = 0;

/* Game state */
int gameover = 0;
int headX, headY;
int fruitX, fruitY;
int score = 0;
static int seed = 180081;

int tailX[MAP_XWIDTH * MAP_YHEIGHT];
int tailY[MAP_XWIDTH * MAP_YHEIGHT];
int tailLength = 2;

int dir = DIR_RIGHT;
GameState gameState = STATE_START;

// Get input from switches (4 LSB for direction) and return corresponding direction
int get_input()
{
    int SwState = get_sw();
    int dir = DIR_NONE;

    // Set direction based on switch input
    switch (SwState & 0b1111)
    {
    case 0b1000:
        dir = DIR_LEFT;
        break;
    case 0b0100:
        dir = DIR_UP;
        break;
    case 0b0010:
        dir = DIR_DOWN;
        break;
    case 0b0001:
        dir = DIR_RIGHT;
        break;
    default:
        break;
    }
    return dir;
}

// Separate function to only get state related input
int get_state_input(){
    int SwState = get_sw();
    int input = DIR_NONE;

    // Set draw mode based on switch input
    if(SwState & 0b1000000000)isoDrawMode = 1;
    else isoDrawMode = 0;

    // Check for start/reset
    switch (SwState & 0b110000)
    {
    case 0b010000:
        input = START_GAME;
        break;
    case 0b100000:
        input = RESET_GAME;
        break;
    default:
        break;
    }
    return input;
}

// Validate direction change (to prevent 180-degree reversals)
int valid_dir(int input)
{
    if (input == DIR_UP || input == DIR_DOWN || input == DIR_LEFT || input == DIR_RIGHT)
    {
        /* Prevent 180-degree reversal */
        if (!((dir == DIR_UP && input == DIR_DOWN) ||
              (dir == DIR_DOWN && input == DIR_UP) ||
              (dir == DIR_LEFT && input == DIR_RIGHT) ||
              (dir == DIR_RIGHT && input == DIR_LEFT)))
        {
            return 1;
        }
    }
    return 0;
}

// Move the snake based on the current direction and update tail positions
void move_snake()
{
    // Move tail
    for (int i = tailLength - 1; i > 0; i--)
    {
        tailX[i] = tailX[i - 1];
        tailY[i] = tailY[i - 1];
    }
    tailX[0] = headX;
    tailY[0] = headY;

    // Move head
    if (dir == DIR_UP)
        headY--;
    else if (dir == DIR_DOWN)
        headY++;
    else if (dir == DIR_LEFT)
        headX--;
    else if (dir == DIR_RIGHT)
        headX++;
}

/* Psuedo random generator */
int get_random(int limit)
{
    seed = (87 * (seed) + dir + headX + headY) % 83647;
    int random = seed % limit;
    return random;
}

// Check if the snake's head has collided with the fruit
int check_fruit_collision()
{
    if (headX == fruitX && headY == fruitY)
    {
        return 1;
    }
    return 0;
}

// Validate that the new fruit position does not overlap with the snake
int valid_fruit_pos()
{
    if (fruitX == headX && fruitY == headY)
        return 0;
    for (int i = 0; i < tailLength - 1; i++)
    {
        if (fruitX == tailX[i] && fruitY == tailY[i])
            return 0;
    }
    return 1;
}

// Generate a new valid position for the fruit
void new_fruit_pos()
{
    // If the snake fills the entire grid, set gameover
    if (MAP_XWIDTH * MAP_YHEIGHT == tailLength)
        gameover = 1;
    
    fruitX = get_random(MAP_XWIDTH);
    fruitY = get_random(MAP_YHEIGHT);
    while (!valid_fruit_pos())
    {
        fruitX = get_random(MAP_XWIDTH);
        fruitY = get_random(MAP_YHEIGHT);
    }
}

// Check for collisions with walls or the snake's own tail
void check_snake_collision()
{
    // Check head into walls
    if (headX < 0 || headX >= MAP_XWIDTH || headY < 0 || headY >= MAP_YHEIGHT)
        gameover = 1;

    // Check head into tail collision
    for (int i = 0; i < tailLength; i++)
    {
        if (tailX[i] == headX && tailY[i] == headY)
            gameover = 1;
    }
}

// Clear the grid map
void clear_grid()
{
    for (int i = 0; i < MAP_XWIDTH; i++)
    {
        for (int j = 0; j < MAP_YHEIGHT; j++)
        {
            gridmap[i][j] = 0;
        }
    }
}

// Add fruit to the grid map
// Fruit represented by number 3
void grid_add_fruit()
{
    gridmap[fruitX][fruitY] = 3;
}

// Add snake to the grid map
// Head represented by 1 and tail by 2
void grid_add_snake()
{
    for (int i = 0; i < tailLength; i++)
    {
        gridmap[tailX[i]][tailY[i]] = 2;
    }
    gridmap[headX][headY] = 1;
}

/* Drawing functions */
// Draw rectangle at grid position
void draw_rect_to_screen(int x, int y, int color)
{
    draw_rect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE, (unsigned char)color);
}

// Draw isometric tile at grid position
void draw_tile_to_screen(int gridX, int gridY, int borderColor, int innerColor)
{
    // Isometric calculation / tranformation off cordinates
    int px = (gridX - gridY) * FLOOR_FRAME_WIDTH / 2 + OFFSET_X;
    int py = (gridX + gridY) * FLOOR_FRAME_HEIGHT / 2 + OFFSET_Y;

    draw_tile(px, py, borderColor, innerColor);
}

// Draw isometric cube at grid position
void draw_cube_to_screen(int gridX, int gridY, int borderColor, int innerColor)
{
    // Isometric calculation / tranformation off cordinates
    int px = (gridX - gridY) * FLOOR_FRAME_WIDTH / 2 + OFFSET_X;
    int py = (gridX + gridY) * FLOOR_FRAME_HEIGHT / 2 + OFFSET_Y - CUBE_FRAME_HEIGHT / 2;

    draw_cube(px, py, borderColor, innerColor);
}

// Draw the entire grid to the screen based on the current grid map
void draw_grid_to_screen()
{
    // clear screen from previous drawing method
    if (isoDrawMode != previousDrawMode)
    {
        fill(0x00);
        previousDrawMode = isoDrawMode;
    }

    // Draw border cube walls
    // replaces the need for a fill (more efficient)
    // otherwise remenants form previous draws remain
    if (isoDrawMode)
    {
        for (int i = -1; i < MAP_XWIDTH; i++)
        {
            for (int j = -1; j < MAP_YHEIGHT; j++)
            {
                if (i == -1 || j == -1)
                    draw_cube_to_screen(i, j, 0x00, 0x49);
            }
        }
    }

    // Draw grid elements
    for (int i = 0; i < MAP_XWIDTH; i++)
    {
        for (int j = 0; j < MAP_YHEIGHT; j++)
        {
            switch (gridmap[i][j])
            {
            case 0:
                // draw_floor_tile(i, j);
                if (isoDrawMode)
                    draw_tile_to_screen(i, j, 0x00, 0x92);
                else
                    draw_rect_to_screen(i, j, 0x49);
                break;
            case 1:
                // draw_snake_head(i, j);
                if (isoDrawMode)
                    draw_cube_to_screen(i, j, 0x0C, 0x10);
                else
                    draw_rect_to_screen(i, j, 0x0C);
                break;
            case 2:
                // draw_snake_tail(i, j);
                if (isoDrawMode)
                    draw_cube_to_screen(i, j, 0x0C, 0x14);
                else
                    draw_rect_to_screen(i, j, 0x10);
                break;
            case 3:
                // draw_fruit(i, j);
                if (isoDrawMode)
                    draw_cube_to_screen(i, j, 0xA0, 0xC0);
                else
                    draw_rect_to_screen(i, j, 0xC0);
                break;
            default:
                break;
            }
        }
    }
}

// Draw the entire game frame
void draw_game_frame()
{
    clear_grid();
    grid_add_fruit();
    grid_add_snake();
    draw_grid_to_screen();
}

// Update the score display on the 7-segment displays
void update_score_display()
{
    int temp_score = score;
    for (int i = 0; i < 6; i++)
    {
        set_displays(i, temp_score % 10);
        temp_score /= 10;
    }
}

// Initialize the game upon start or reset
void game_init(void)
{

    gameState = STATE_START;
    gameover = 0;
    score = 0;
    update_score_display();
    tailLength = 2;
    dir = DIR_RIGHT;
    fill(0x00);
    headX = MAP_XWIDTH / 2;
    headY = MAP_YHEIGHT / 2;
    // Initialize tail positions to prevent drawing issues
    for (int i = 0; i < tailLength; i++)
    {
        tailX[i] = headX;
        tailY[i] = headY;
    }

    new_fruit_pos();
    draw_image(start);
}

// Main game tick function to handle game state updates and game logic
void game_tick(void)
{
    switch (gameState)
    {
    case STATE_START: // Show start screen and wait for input to start the game
        draw_image(start);
        if (get_state_input() == START_GAME)
        {
            gameState = STATE_PLAYING;
            fill(0x00);
            draw_game_frame();
        }
        break;

    case STATE_PLAYING: // Main gameplay logic
    {
        int input = get_input();
        get_state_input(); // to update isoDrawMode even during gameplay
        if (valid_dir(input))
        {
            dir = input;
        }
        move_snake();

        check_snake_collision();

        if (check_fruit_collision())
        {
            new_fruit_pos();
            score++;
            update_score_display();
            // Increase tail length
            if (tailLength < MAP_XWIDTH * MAP_YHEIGHT)
            {
                tailLength++;
                tailX[tailLength - 1] = tailX[0];
                tailY[tailLength - 1] = tailY[0];
            }
        }

        if (!gameover)
        {
            draw_game_frame();
        }
        else
        {
            gameState = STATE_GAMEOVER;
            draw_image(end);
        }
        break;
    }

    case STATE_GAMEOVER: // Show game over screen and wait for reset input
        draw_image(end);
        if (get_state_input() == RESET_GAME)
        {
            game_init();
        }
        break;
    }
}
