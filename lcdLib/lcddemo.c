/** \file lcddemo.c
 *  \brief A simple demo that draws a string and square
 */

#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"

void makeCircle(short col, short row, short size, short color);

/** Initializes everything, clears the screen, draws "hello" and a square */
int
main()
{
  configureClocks();
  lcd_init();
  u_char width = screenWidth, height = screenHeight;

  clearScreen(COLOR_BLACK);
  
  drawString5x7(30,30, "Kary te amo", COLOR_PINK, COLOR_BLACK);

  short rowMiddle = screenHeight/2;
  short colMiddle = screenWidth/2;
  int minCol;
  int maxCol;
  short row, col; //row =   col ||
  for (int j = 0; j < 20; j++){
    row = j;
    minCol = -j; // negative of the value of j EX -(20) |||
    maxCol = j; //the limit for the colums |||
    for (col = minCol; col <= maxCol; col++) {
      drawPixel(col + colMiddle, rowMiddle - row, COLOR_RED);  
    }
  }
  
  //
  // made half of circles to create the top of the heart
  //
  
  char allComplete = 0;  //check if heart is done
  char leftComplete = 0; //check if left is done
  short count = 2;  // create the top of the circle
  short rowMiddleCircle = rowMiddle - 29; // set the row point where to create the circle
  short colMiddleCircle = colMiddle - 10; 

 top:
  for (int j = 0; j < 10; j++){
    
    if (leftComplete){  // once left is complete do right 
      count = 2;
      colMiddleCircle = colMiddle + 10;
      goto drawMidCircle;
    } else {
      goto drawMidCircle;
    }
    
  drawMidCircle:  //by defaut will start to draw left half of circle
    row = j;
    minCol = -(j + count);
    maxCol = j + count;
    for (col = minCol; col <= maxCol; col++) {
      drawPixel(colMiddleCircle + col, rowMiddleCircle + row, COLOR_RED);  
    }
    if (j == 1) count++;
    if (j == 5) count--;
    if (j == 7) count--;
    if (leftComplete){
      allComplete = 1;
      leftComplete = 0;
    }
  }
  if (!allComplete){
    leftComplete = 1;
    goto top;
  }
}


