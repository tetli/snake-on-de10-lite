/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */

/* Below functions are external and found in other files. */

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

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

static int screen[SCREEN_WIDTH][SCREEN_HEIGHT] = {0};

#define MAP_XWIDTH 12
#define MAP_YHEIGHT 12

static int gridmap[MAP_XWIDTH][MAP_YHEIGHT] = {0};

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

int tailX[MAP_XWIDTH*MAP_YHEIGHT];
int tailY[MAP_XWIDTH*MAP_YHEIGHT];
int tailLength = 0;

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

void update_time(){
  previousTime = mytime;
  if (TIMER_STATUS & 0x1)
  {
    TIMER_STATUS = 0x0;
    timeoutcount++;
    timeoutcount %= 10;

    if (timeoutcount == 0)
    {
      tick(&mytime); // Ticks the clock once
      if (((previousTime & 0xFF00) != 0) && ((mytime & 0xFF00) == 0))
      {
        hourCount++;
        if (hourCount >= 24)
        {
          hourCount = 0;
        }
      }
      update_display_time(mytime, hourCount);
    }
    // Sätter timer status till 0. kan andänra hela värdet eftersom RUN biten ändras inte när den skrivs till.
  }
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
        /* prevent 180-degree reversal */
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


int check_fruit_collision(){
  if (headX == fruitX && headY == fruitY) {
        fruitX = get_random(MAP_XWIDTH);
        fruitY = get_random(MAP_YHEIGHT);
        return 1;
    }
  return 0;
}

void check_snake_collision(){
  // Check walls
  if (headX < 0 || headX >= MAP_XWIDTH || headY < 0 || headY >= MAP_YHEIGHT)
      gameover = 1;

  // Check tail collision
  for (int i = 0; i < tailLength; i++) {
    if (tailX[i] == headX && tailY[i] == headY)
      gameover = 1;
    }
}

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause)
{
  if(cause == 16){
    update_time();
    int input = get_input();
    if (valid_dir(input)){
      dir = input;
    }
    move_snake();

    check_snake_collision();
    if (check_fruit_collision()){
      score++;
      if(tailLength<MAP_XWIDTH*MAP_YHEIGHT)tailLength++;
    }

    draw_stage();
    draw_fruit();
    draw_snake_tail();
    draw_snake_head();
    draw_screen_to_fb();
    
  }
}

/* Set timer to input (milliseconds)*/
void set_timer(int time){
    time *= 30000; // Konvertera time till rätt format. 30MHz 
    int timeLower = time%(1<<16);
    int timeUpper = (time >>16)%(1<<16);
    TIMER_PERIODL = timeLower;
    TIMER_PERIODH = timeUpper;
}

/* Initializing game and interrupts. */
void gameinit(void)
{
  enable_interrupt();

  // sätt periodL och persiodH till 3 miljoner. mostvarar 100ms
  set_timer(3000000);
  TIMER_CONTROL = 0b0111;
}



int main(void)
{
  gameinit();

  int count;
  while (gameover == 0)
  {
    if(timeoutcount >= 10){
      timeoutcount = 0;
      count = (count + 1)%10;
      set_displays(1, count);
    }

  }
}
