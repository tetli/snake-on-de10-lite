#ifndef GAME_H
#define GAME_H

#define MAP_XWIDTH 16
#define MAP_YHEIGHT 12

/* Directions */
#define DIR_NONE 0
#define DIR_UP 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define DIR_RIGHT 4

void game_init(void);
void game_tick(void);
int get_input(void);

#endif
