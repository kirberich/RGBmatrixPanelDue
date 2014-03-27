// example to draw all 9-bit colors! public domain

#include "RGBmatrixPanelDue.h"

// xpanels, ypanels, nplanes (tested w/3)
uint8_t nX = 2;
uint8_t nY = 1;
RGBmatrixPanelDue matrix(nX, nY, 3);

// wrapper for the redrawing code, this gets called by the interrupt
//TIMER: TC1 ch 0
void TC3_Handler()
{
  TC_GetStatus(TC1, 0);
  matrix.updateDisplay();
}

void setup() {
  Serial.begin(9600);

  matrix.begin(5000);
  
  uint8_t r=0, g=0, b=0;
  for (uint8_t x=0; x < 32*nX; x++) {      
    for (uint8_t y=0; y < 8*nY; y++) {  
      matrix.drawPixel(x, y, matrix.Color333(r, g, b));
      r++;
      if (r == 8) {
        r = 0; g++;
      }
      if (g == 8) {
        g = 0; b++;
      }
    }
  }

  for (uint8_t x=0; x < 32*nX; x++) {      
    for (uint8_t y=8; y < 16*nY; y++) {  
      matrix.drawPixel(x, y, matrix.Color333(r, g, b));
      r++;
      if (r == 8) {
        r = 0; g++;
      }
      if (g == 8) {
        g = 0; b++;
      }
    }
  }
}

void loop() {
  // do nothing
}

