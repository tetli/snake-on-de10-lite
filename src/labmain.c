/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */

/* Below functions are external and found in other files. */

#include "floorSprite.h"

extern void enable_interrupt(void);

#define TIMER_BASE 0x04000020

#define TIMER_STATUS (*(volatile int *)(TIMER_BASE + 0x00))
#define TIMER_CONTROL (*(volatile int *)(TIMER_BASE + 0x04))
#define TIMER_PERIODL (*(volatile int *)(TIMER_BASE + 0x08))
#define TIMER_PERIODH (*(volatile int *)(TIMER_BASE + 0x0C))
#define TIMER_SNAPL (*(volatile int *)(TIMER_BASE + 0x10))
#define TIMER_SNAPH (*(volatile int *)(TIMER_BASE + 0x14))

int mytime = 0x5957;

int timeoutcount = 0;

volatile int previousTime = 0;
volatile int hourCount = 0;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FB_BASE 0x08000000

static int screen[SCREEN_WIDTH][SCREEN_HEIGHT] = {0};

#define MAP_XWIDTH 12
#define MAP_YHEIGHT 12

static int gridmap[MAP_XWIDTH][MAP_YHEIGHT] = {0};

#define OFFSET_X (SCREEN_WIDTH / 2)  // center horizontally
#define OFFSET_Y 50 // distance from the top

/* "Random" seed */

/* Directions */
#define DIR_NONE  0
#define DIR_UP    1
#define DIR_DOWN  2
#define DIR_LEFT  3
#define DIR_RIGHT 4

/* Game state */
int gameover = 0;
int headX, headY;
int fruitX, fruitY;
int score = 0;
static int seed = 180081;


int tailX[MAP_XWIDTH*MAP_YHEIGHT];
int tailY[MAP_XWIDTH*MAP_YHEIGHT];
int tailLength = 1;

int dir = DIR_RIGHT;


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

int get_input(){
  int SwState = get_sw() & 0b1111;
  int dir = DIR_NONE;
  switch (SwState)
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

int valid_dir(int input){
  if (input == DIR_UP || input == DIR_DOWN || input == DIR_LEFT || input == DIR_RIGHT) {
        /* Prevent 180-degree reversal */
        if (!((dir == DIR_UP && input == DIR_DOWN) ||
              (dir == DIR_DOWN && input == DIR_UP) ||
              (dir == DIR_LEFT && input == DIR_RIGHT) ||
              (dir == DIR_RIGHT && input == DIR_LEFT))) {
            return 1;
        }
  }
  return 0;
}

void move_snake(){
  // Move tail
  for(int i=tailLength-1; i>0; i--){
    tailX[i] = tailX[i-1];
    tailY[i] = tailY[i-1];
  }
  tailX[0] = headX;
  tailY[0] = headY;

  // Move head
    if (dir == DIR_UP) headY--;
    else if (dir == DIR_DOWN) headY++;
    else if (dir == DIR_LEFT) headX--;
    else if (dir == DIR_RIGHT) headX++;
  
}
/* Psuedo random generator */
int get_random(int limit){
  seed = (87 * (seed) + dir + headX + headY) % 83647;
  int random = seed % limit;
  return random;
}

int check_fruit_collision(){
  if (headX == fruitX && headY == fruitY) {
        return 1;
  }
  return 0;
}

int valid_fruit_pos(){
  if(fruitX == headX && fruitY == headY) return 0;
  for(int i = 0; i < tailLength-1;i++){
    if(fruitX == tailX[i] && fruitY == tailY[i]) return 0;
  }
  return 1;
}

void new_fruit_pos(){
  if (MAP_XWIDTH*MAP_YHEIGHT == tailLength) gameover = 1;
  fruitX = get_random(MAP_XWIDTH);
  fruitY = get_random(MAP_YHEIGHT);
  while(!valid_fruit_pos()){
    fruitX = get_random(MAP_XWIDTH);
    fruitY = get_random(MAP_YHEIGHT);
  }
}

void check_snake_collision(){
  // Check head into walls
  if (headX < 0 || headX >= MAP_XWIDTH || headY < 0 || headY >= MAP_YHEIGHT)
      gameover = 1;

  // Check head into tail collision
  for (int i = 0; i < tailLength; i++) {
    if (tailX[i] == headX && tailY[i] == headY)
      gameover = 1;
    }
}

void clear_grid(){
  for (int i = 0; i < MAP_XWIDTH; i++)
  {
    for(int j = 0; j < MAP_YHEIGHT; j++){
      gridmap[i][j] = 0;
    }
  } 
}

// Fruit represented by number 3
void grid_add_fruit(){
  gridmap[fruitX][fruitY] = 3;
}

// Head represented by 1 and tail by 2
void grid_add_snake(){
  gridmap[headX][headY] = 1;

  for (int i = 0; i < tailLength; i++)
  {
    gridmap[tailX[i]][tailY[i]] = 2;
  }
}

// void draw_floor_tile(int x, int y){}

// void draw_snake_head(int x, int y){}

// void draw_snake_tail(int x, int y){}

// void draw_fruit(int x, int y){}

void draw_rect_to_screen(int x, int y, int color){
  int px = x * 20;
  int py = y * 20;
  for(int i=0; i < 20; i++)
  {
    for (int j = 0; j < 20; j++)
    {
      screen[px+i][py+j] = color;
    }
  }
}


void draw_grid_to_screen(){
  for (int i = 0; i < MAP_XWIDTH; i++)
  {
    for(int j = 0; j < MAP_YHEIGHT; j++){
      switch (gridmap[i][j])
      {
      case 0:
        //draw_floor_tile(i, j);
        draw_rect_to_screen(i,j,0x49);
        break;
      case 1:
        //draw_snake_head(i, j);
        draw_rect_to_screen(i,j,0x0C);
        break;
      case 2:
        //draw_snake_tail(i, j);
        draw_rect_to_screen(i,j,0x10);
        break;
      case 3:
        //draw_fruit(i, j);
        draw_rect_to_screen(i,j,0xC0);
        break;
      default:
        break;
      }
    }
  } 
}

void draw_pixel(int x, int y, int color)
{
    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return;

    // framebuffer pointer
    volatile unsigned char *fb = (volatile unsigned char *)FB_BASE;

    fb[y * SCREEN_WIDTH + x] = (unsigned char)color; // unsigned char is 8bit, same as the display
}
void draw_screen_to_fb(){
  for (int y = 0; y < SCREEN_HEIGHT; y++)
  {
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
      draw_pixel(x,y, screen[x][y]);
    }
  }
}





/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause)
{
  if(cause == 16){
    TIMER_STATUS = 0x0;
    timeoutcount++;
    //update_time();
    int input = get_input();
    if (valid_dir(input)){
      dir = input;
    }
    move_snake();

    check_snake_collision();

    if (check_fruit_collision()){
      new_fruit_pos();
      score++;
      // set_displays(0, score);
      if(tailLength<MAP_XWIDTH*MAP_YHEIGHT){
        tailLength++;
        tailX[tailLength-1] = tailX[0];
        tailY[tailLength-1] = tailY[0];
      }
    }

    if (!gameover){
      clear_grid();
      grid_add_fruit();
      grid_add_snake();
      draw_grid_to_screen();
      draw_screen_to_fb();
    }
    else
      TIMER_CONTROL &= ~0x1;
  }
}

/* Set timer to input (milliseconds)*/
void set_timer(int time){
    unsigned int time_val = (unsigned int)time * 30000U; // // Konvertera time till rätt format. 30MHz 
    unsigned int timeLower = time_val & 0xFFFF;
    unsigned int timeUpper = (time_val >> 16) & 0xFFFF;
    TIMER_PERIODL = timeLower;
    TIMER_PERIODH = timeUpper;
}

/* Initializing game and interrupts. */
void gameinit(void)
{
  headX = MAP_XWIDTH/2;
  headY = MAP_YHEIGHT/2;
  for(int i = 0; i<tailLength;i++){
    tailX[i] = headX;
    tailY[i] = headY;
  }
  
  new_fruit_pos();


  // sätt periodL och persiodH till 1000ms
  set_timer(1000);
  TIMER_CONTROL = 0b0111;

  enable_interrupt();
}



int main(void)
{
  gameinit();
}