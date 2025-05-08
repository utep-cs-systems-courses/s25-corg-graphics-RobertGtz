#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"

#define LED BIT6

#define SW1 1
#define SW2 2
#define SW3 4
#define SW4 8
#define SWITCHES 15

#define SHIP_W 10
#define SHIP_H 5

#define ASTEROID_COUNT 3
#define ASTEROID_SPEED 1

typedef struct {
  int col, row, active;
} Asteroid;

Asteroid asteroids[ASTEROID_COUNT];

//game

u_char shipCol = 60;
int score = 0;

char game_over = 0;

//window
char current_position = 0;
volatile short redrawScreen = 0;

int switches = 0;

//generate ramdoms numbers
int random() {
  static unsigned int seed = 0;
  seed = (seed * 1103515245 + 12345 + TAR) & 0x7FFF;
  return (seed % (screenWidth - 10));
}

//SHIP
void drawShip() {
  fillRectangle(shipCol, screenHeight - SHIP_H - 2, SHIP_W, SHIP_H, COLOR_WHITE);
}
void undrawShip() {
  fillRectangle(shipCol, screenHeight - SHIP_H - 2, SHIP_W, SHIP_H, COLOR_BLACK);
}


//if = change button to the new buttom push
void switch_update_interrupt_sense() {

  char p2val = P2IN;
  P2IES |= (p2val & SWITCHES);/* if switch up, sense down */
  P2IES &= (p2val | ~SWITCHES);/* if switch down, sense up */
  //return p2val; //What buttom was push

}

//initialize buttons 
void switch_init() {

  P2REN |= SWITCHES;/* enables resistors for switches */
  P2IE |= SWITCHES;/* enable interrupts from switches */
  P2OUT |= SWITCHES;/* pull-ups for switches */
  P2DIR &= ~SWITCHES;/* set switches' bits for input */

  switch_update_interrupt_sense();

}
volatile char switch_pressed = 0;

//to know what buttom was press and then do something with it
void switch_interrupt_handler() {

  char p2val = P2IN;
  //switch_update_interrupt_sense();
  switch_pressed = ~p2val & SWITCHES; //turn on only the button push
  P2IFG &= ~SWITCHES;
  switch_update_interrupt_sense();
  redrawScreen = 1;
  /*
  if (switches & SW1) {
    if (shipCol > 5) {
      undrawShip();
      shipCol -= 5;
      drawShip();
    }
  }
  if (switches & SW2) {
    if (shipCol < (screenWidth - (SHIP_W - 5))) {
      undrawShip();
      shipCol += 5;
      drawShip();
    }
  }*/
  /*
  if (switches & SWITCHES) {
    redrawScreen = 1;

    for (char swNum = 0; swNum < 4; swNum++) { 
      int swFlag = 1 << swNum;

      if (switches & swFlag) {
	current_position = swNum;
	break;
      }
    }
  }*/
}
//game
void init_asteroids() {
  for (int i = 0; i < ASTEROID_COUNT; i++) {
    asteroids[i].col = random() % (screenWidth - 10);
    asteroids[i].row = - (i * 20);
    asteroids[i].active = 1;
    fillRectangle(asteroids[i].col, asteroids[i].row, 5, 5, COLOR_RED);
  }
}


void move_asteroids() {
  for (int i = 0; i < ASTEROID_COUNT; i++) {
    if (asteroids[i].active) {
      fillRectangle(asteroids[i].col, asteroids[i].row, 5, 5, COLOR_BLACK);
      asteroids[i].row += ASTEROID_SPEED;
      if (asteroids[i].row > screenHeight) {
	asteroids[i].col = random();
	asteroids[i].row = 0;
	score++;
      }
      fillRectangle(asteroids[i].col, asteroids[i].row, 5, 5, COLOR_RED);
    }
  }
}

int collision() {
  int i = 0;
  while (i < ASTEROID_COUNT) {
    if (asteroids[i].active) {
      int asteroidBottom = asteroids[i].row + 5;
      int shipTop = screenHeight - SHIP_H - 2;
      if (asteroidBottom >= shipTop
   	  && asteroids[i].col + 5 >= shipCol
    	  && asteroids[i].col <= shipCol + SHIP_W) {
    	return 1;
      }
    }
    i++;
  }
  return 0;
}

void intToStr(int num, char *str) {
  int i = 0;
  if (num == 0) {
    str[i++] = '0';
  } else {
    while (num > 0) {
      str[i++] = (num % 10) + '0';
      num /= 10;
    }
    // Reversar el string
    for (int j = 0; j < i / 2; ++j) {
      char temp = str[j];
      str[j] = str[i - j - 1];
      str[i - j - 1] = temp;
    }
  }
  str[i] = '\0'; // Termina la cadena
}

void show_score() {
  char scoreStr[16] = "score: ";
  intToStr(score, scoreStr + 7);
  drawString5x7(0, 0, scoreStr, COLOR_WHITE, COLOR_BLACK);
}

//window
u_int getColor(int count){
  //state machine
  switch ((count / 50) % 6) {
  case 0: return COLOR_RED;
  case 1: return COLOR_ORANGE;
  case 2: return COLOR_YELLOW;
  case 3: return COLOR_PINK;
  case 4: return COLOR_GREEN;
  case 5: return COLOR_PURPLE;
  }
  return COLOR_WHITE;
}

void drawWithDelay(int col, int row, int *count) {
  u_int color = getColor(*count);
  drawPixel(col, row, color);
  (*count)++;
  if ((*count) % 5 == 0) {
    __delay_cycles(1000);
  }
}

void close_Window() {

  int left = 0;
  int right = screenWidth - 1;
  int top = 0;
  int bottom = screenHeight - 1;
  int col = 0;
  int row = 0;
  int count = 0;
  
  while (left <= right && top <= bottom) {

    // goes down
    while (row <= bottom) {
      drawWithDelay(col, row, &count); //passing &count address so it get change from everywhere
      row++;
    }
    row--;
    left++; // mov right
    col++;
    
    // goes right
    while (col <= right) {
      drawWithDelay(col, row, &count);
      col++;
    }
    col--;
    bottom--; // go to up 
    row--;

    // goes up
    while (row >= top) {
      drawWithDelay(col, row, &count);
      row--;
    }
    row++;
    right--; // mov left
    col--;

    // goes left
    while (col >= left) {
      drawWithDelay(col, row, &count);
      col--;
    }
    col++;
    top++; // move down a row
    row++;
  }
}
volatile int counters = 0;
void wdt_c_handler() {
  //P1OUT ^= LED;
  counters++;
  if (counters >= 250) {
    counters = 0;
    redrawScreen = 1;
    P1OUT ^= LED;
  }
}

int main() {
  P1DIR |= LED;
  P1OUT |= LED;
  
  configureClocks();
  lcd_init();
  switch_init();
  
  clearScreen(COLOR_BLACK);

  shipCol = 60;
  score = 0;
  game_over = 0;
  
  init_asteroids();
  drawShip();
  show_score();
  
  enableWDTInterrupts();
  or_sr(0x8);

  while (1) {
    if (redrawScreen) {
      redrawScreen = 0;
      if (switch_pressed) {
	if (switch_pressed & SW1) {
	  if (shipCol > 5) {
	    undrawShip();
	    shipCol -= 5;
	    drawShip(); 
	  }
	}
	
	if (switch_pressed & SW2) {
	  if (shipCol < (screenWidth - SHIP_W - 5)) {
	    undrawShip();
	    shipCol += 5;
	    drawShip(); 
	  }
	}
	switch_pressed = 0;
      }	
      move_asteroids();
      show_score();
      
      if (collision()) {
	clearScreen(COLOR_BLACK);
	drawString5x7(30, screenHeight / 2, "GAME OVER", COLOR_RED, COLOR_BLACK);
	while(1);
      }
    }
  }
}

void __interrupt_vec(PORT2_VECTOR) Port_2() {
  if (P2IFG & SWITCHES) {
    //P2IFG &= ~SWITCHES;
    switch_interrupt_handler();
  }
}
