// demo for all the abilities of the RGBmatrixPanelDue library. public domain!

#include "RGBmatrixPanelDue.h"

// xpanels, ypanels, nplanes (tested w/3)
uint8_t nXpanels = 2;
uint8_t nYpanels = 3;
RGBmatrixPanelDue matrix(nXpanels, nYpanels, 3);
//RGBmatrixPanelDue matrix(nXpanels, nYpanels);


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

  matrix.begin(1000);
  
  // draw a pixel in solid white
  //               x, y
  //matrix.drawPixel(65, 1, matrix.Color333(7, 0, 0)); 

  matrix.drawPixel(33, 32, matrix.Color333(7, 0, 0)); 

  // whew!
}

void loop() {
  // do nothing
}

