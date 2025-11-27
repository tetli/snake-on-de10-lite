#ifndef GAME_H
#define GAME_H

#include "screen.h"

#define GRID_SIZE 10
#define MAP_XWIDTH (SCREEN_WIDTH / GRID_SIZE)
#define MAP_YHEIGHT (SCREEN_HEIGHT / GRID_SIZE)

/* Directions */
#define DIR_NONE 0
#define DIR_UP 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define DIR_RIGHT 4
#define START_GAME 5
#define RESET_GAME 6

typedef enum
{
    STATE_START,
    STATE_PLAYING,
    STATE_GAMEOVER
} GameState;

void game_init(void);
void game_tick(void);
int get_input(void);

#endif
