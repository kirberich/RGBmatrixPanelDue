// demo for all the abilities of the RGBmatrixPanelDue library. public domain!

#include "RGBmatrixPanelDue.h"
//#include "Arduino.h"

// xpanels, ypanels, nplanes (tested w/3)
uint8_t nXpanels = 2;
uint8_t nYpanels = 3;
RGBmatrixPanelDue matrix(nXpanels, nYpanels, 3);

// wrapper for the redrawing code, this gets called by the interrupt
//TIMER: TC1 ch 0
void TC3_Handler()
{
  TC_GetStatus(TC1, 0);
  matrix.updateDisplay();
}

void setup() {
  
  //nYpanels = 1;
  Serial.begin(9600);

  matrix.begin(3000);
  
  // draw a pixel in solid white
  matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7)); 
  delay(2000);

  // fix the screen with green
  matrix.fillRect(0, 0, 32*nXpanels, 16*nYpanels, matrix.Color333(0, 7, 0));
  delay(2000);

  // draw a box in yellow
  matrix.drawRect(0, 0, 32*nXpanels, 16*nYpanels, matrix.Color333(7, 7, 0));
  delay(2000);
  
  // draw an 'X' in red
  matrix.drawLine(0, 0, 31*nXpanels, 15*nYpanels, matrix.Color333(7, 0, 0));
  matrix.drawLine(31*nXpanels, 0, 0, 15*nYpanels, matrix.Color333(7, 0, 0));
  delay(2000);
  
  // draw a blue circle
  matrix.drawCircle(7, 7, 7, matrix.Color333(0, 0, 7));
  delay(2000);
  
  // fill a violet circle
  matrix.fillCircle(23*nXpanels, 7*nYpanels, 7, matrix.Color333(7, 0, 7));
  delay(2000);
  
  // fill the screen with 'black'
  matrix.fill(matrix.Color333(0, 0, 0));
  
  // draw some text!
  matrix.setCursor(1, 0);   // start at top left, with one pixel of spacing
  matrix.setTextSize(1);    // size 1 == 8 pixels high
  
  // print each letter with a rainbow color
  matrix.setTextColor(matrix.Color333(7,0,0));
  matrix.writeAChar('1');
  matrix.setTextColor(matrix.Color333(7,4,0)); 
  matrix.writeAChar('6');
  matrix.setTextColor(matrix.Color333(7,7,0));
  matrix.writeAChar('x');
  matrix.setTextColor(matrix.Color333(4,7,0)); 
  matrix.writeAChar('3');
  matrix.setTextColor(matrix.Color333(0,7,0));  
  matrix.writeAChar('2');
  
  matrix.setCursor(1, 9);   // next line
  matrix.setTextColor(matrix.Color333(0,7,7)); 
  matrix.writeAChar('*');
  matrix.setTextColor(matrix.Color333(0,4,7)); 
  matrix.writeAChar('R');
  matrix.setTextColor(matrix.Color333(0,0,7));
  matrix.writeAChar('G');
  matrix.setTextColor(matrix.Color333(4,0,7)); 
  matrix.writeAChar('B');
  matrix.setTextColor(matrix.Color333(7,0,4)); 
  matrix.writeAChar('*');

  // whew!
}

void loop() {
  // do nothing
}

