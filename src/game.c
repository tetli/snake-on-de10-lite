#include "game.h"
#include "board.h"
#include "screen.h"
#include "start.h"

static int gridmap[MAP_XWIDTH][MAP_YHEIGHT] = {0};

/* Game state */
int gameover = 0;
int headX, headY;
int fruitX, fruitY;
int score = 0;
static int seed = 180081;

int tailX[MAP_XWIDTH * MAP_YHEIGHT];
int tailY[MAP_XWIDTH * MAP_YHEIGHT];
int tailLength = 1;

int dir = DIR_RIGHT;

int get_input()
{
    int SwState = get_sw() & 0b111111;
    int dir = DIR_NONE;
    switch (SwState)
    {
    case 0b100000:
        dir = RESET_GAME;
        break;
    case 0b10000:
        dir = START_GAME;
        break;
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

int check_fruit_collision()
{
    if (headX == fruitX && headY == fruitY)
    {
        return 1;
    }
    return 0;
}

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

void new_fruit_pos()
{
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

// Fruit represented by number 3
void grid_add_fruit()
{
    gridmap[fruitX][fruitY] = 3;
}

// Head represented by 1 and tail by 2
void grid_add_snake()
{
    gridmap[headX][headY] = 1;

    for (int i = 0; i < tailLength; i++)
    {
        gridmap[tailX[i]][tailY[i]] = 2;
    }
}

void draw_rect_to_screen(int x, int y, int color)
{
    draw_rect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE, (unsigned char)color);
}

void draw_grid_to_screen()
{
    for (int i = 0; i < MAP_XWIDTH; i++)
    {
        for (int j = 0; j < MAP_YHEIGHT; j++)
        {
            switch (gridmap[i][j])
            {
            case 0:
                // draw_floor_tile(i, j);
                draw_rect_to_screen(i, j, 0x49);
                break;
            case 1:
                // draw_snake_head(i, j);
                draw_rect_to_screen(i, j, 0x0C);
                break;
            case 2:
                // draw_snake_tail(i, j);
                draw_rect_to_screen(i, j, 0x10);
                break;
            case 3:
                // draw_fruit(i, j);
                draw_rect_to_screen(i, j, 0xC0);
                break;
            default:
                break;
            }
        }
    }
}

GameState gameState = STATE_START;

void game_init(void)
{
    gameState = STATE_START;
    gameover = 0;
    score = 0;
    tailLength = 1;
    dir = DIR_RIGHT;

    fill(0);
    headX = MAP_XWIDTH / 2;
    headY = MAP_YHEIGHT / 2;
    for (int i = 0; i < tailLength; i++)
    {
        tailX[i] = headX;
        tailY[i] = headY;
    }

    new_fruit_pos();
}

void game_tick(void)
{
    switch (gameState)
    {
    case STATE_START:
        draw_image(start);
        if (get_input() == START_GAME)
        {
            gameState = STATE_PLAYING;
            fill(0);
        }
        break;

    case STATE_PLAYING:
    {
        int input = get_input();
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
            // set_displays(0, score);
            if (tailLength < MAP_XWIDTH * MAP_YHEIGHT)
            {
                tailLength++;
                tailX[tailLength - 1] = tailX[0];
                tailY[tailLength - 1] = tailY[0];
            }
        }

        if (!gameover)
        {
            clear_grid();
            grid_add_fruit();
            grid_add_snake();
            draw_grid_to_screen();
        }
        else
        {
            gameState = STATE_GAMEOVER;
        }
        break;
    }

    case STATE_GAMEOVER:
        fill(0x01);
        if (get_input() == RESET_GAME)
        {
            game_init();
        }
        break;
    }
}
