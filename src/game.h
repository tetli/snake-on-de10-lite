#ifndef GAME_H
#define GAME_H

#define MAP_XWIDTH 16
#define MAP_YHEIGHT 12
#define GRID_SIZE 20

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
