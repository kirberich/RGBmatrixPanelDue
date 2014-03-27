#include "RGBmatrixPanelDue.h"
#include "glcdfontDue.c"
#include <Arduino.h>

uint8_t RGBmatrixPanelDue::width() {return WIDTH; }

uint8_t RGBmatrixPanelDue::height() {return HEIGHT; }


RGBmatrixPanelDue::RGBmatrixPanelDue(uint8_t xpanels, uint8_t ypanels, uint8_t planes) {

  NX = xpanels;
  NY = ypanels;

  PWMBITS = planes;
  PWMMAX = ((1 << PWMBITS) - 1);

  WIDTH = single_matrix_width*xpanels;
  HEIGHT = single_matrix_height*ypanels;
  NUMBYTES = (WIDTH * HEIGHT / 2) * planes;
  matrixbuff = (uint8_t *)malloc(NUMBYTES);
  // set all of matrix buff to 0 to begin with
  memset(matrixbuff, 0, NUMBYTES);


  pwmcounter = 0;
  scansection = 0;

  cursor_x = cursor_y = 0;
  textsize = 1;
  textcolor = Color333(7,7,7); // white

}


void RGBmatrixPanelDue::begin(uint32_t frequency) {
  pinMode(APIN, OUTPUT);
  digitalWrite(APIN, LOW); 
  pinMode(BPIN, OUTPUT);
  digitalWrite(BPIN, LOW); 
  pinMode(CPIN, OUTPUT);
  digitalWrite(CPIN, LOW); 
  pinMode(LAT, OUTPUT);
  digitalWrite(LAT, LOW); 
  pinMode(CLK, OUTPUT);
  digitalWrite(CLK, HIGH); 

  pinMode(OE, OUTPUT);
  digitalWrite(OE, LOW); 
  
  pinMode(R1,OUTPUT);
  pinMode(R2,OUTPUT);
  pinMode(G1,OUTPUT);
  pinMode(G2,OUTPUT);
  pinMode(B1,OUTPUT);
  pinMode(B2,OUTPUT);
  digitalWrite(R1,LOW);
  digitalWrite(R2,LOW);
  digitalWrite(G1,LOW);
  digitalWrite(G2,LOW);
  digitalWrite(B1,LOW);
  digitalWrite(B2,LOW);

  startTimer(TC1, 0, TC3_IRQn, frequency); //TC1 channel 0, the IRQ for that channel and the desired frequency

  //  Serial.println(WIDTH);
  //Serial.println(HEIGHT);
  //Serial.println(NUMBYTES);
  //Serial.println("end of matrix.begin");

}

uint16_t RGBmatrixPanelDue::Color333(uint8_t r, uint8_t g, uint8_t b) {
  return Color444(r,g,b);
}

uint16_t RGBmatrixPanelDue::Color444(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t c;
  
  c = r;
  c <<= 4;
  c |= g & 0xF;
  c <<= 4;
  c |= b & 0xF;
  return c;
}

uint16_t RGBmatrixPanelDue::Color888(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t c;
  
  c = (r >> 5);
  c <<= 4;
  c |= (g >> 5) & 0xF;
  c <<= 4;
  c |= (b >> 5) & 0xF;

  /*
  Serial.print(r, HEX); Serial.print(", ");
  Serial.print(g, HEX); Serial.print(", ");
  Serial.print(b, HEX); Serial.print("->");
  Serial.println(c, HEX);
  */

  return c;
}



// draw a pixel at the x & y coords with a specific color
void  RGBmatrixPanelDue::drawPixel(uint8_t xin, uint8_t yin, uint16_t c) {
  uint16_t index = 0;
  uint8_t old, x, y;
  uint8_t red, green, blue, panel, ysave, ii;
  
  // extract the 12 bits of color
  red = (c >> 8) & 0xF;
  green = (c >> 4) & 0xF;
  blue = c & 0xF;
  
  // change to right coords
  x = (yin - yin%single_matrix_height)/single_matrix_height*single_matrix_width*NX + xin;
  y = yin%single_matrix_height;

  // both top and bottom are stored in same byte
  if (y%single_matrix_height < 8) 
    index = y%single_matrix_height;
  else 
  index = y%single_matrix_height-8;
  // now multiply this y by the # of pixels in a row
  index *= single_matrix_width*NX*NY;
  // now, add the x value of the row
  index += x;
  // then multiply by 3 bytes per color (12 bit * High and Low = 24 bit = 3 byte)
  index *= PWMBITS;


  old = matrixbuff[index];
  if (y%single_matrix_height < 8) {
    // we're going to replace the high nybbles only
    // red first!
    matrixbuff[index] &= ~0xF0;  // mask off top 4 bits
    matrixbuff[index] |= (red << 4);
    index++;
    // then green
    matrixbuff[index] &= ~0xF0;  // mask off top 4 bits
    matrixbuff[index] |= (green << 4);
    index++;
    // finally blue
    matrixbuff[index] &= ~0xF0;  // mask off top 4 bits
    matrixbuff[index] |= (blue << 4);
  } else {
    // we're going to replace the low nybbles only
    // red first!
    matrixbuff[index] &= ~0x0F;  // mask off bottom 4 bits
    matrixbuff[index] |= red;
    index++;
    // then green
    matrixbuff[index] &= ~0x0F;  // mask off bottom 4 bits
    matrixbuff[index] |= green;
    index++;
    // finally blue
    matrixbuff[index] &= ~0x0F;  // mask off bottom 4 bits
    matrixbuff[index] |= blue;
  }
}



// bresenham's algorithm - thx wikpedia
void RGBmatrixPanelDue::drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1, 
		      uint16_t color) {
  //int8_t y1 = y11;
  //int8_t y0 = y00;
  //int8_t x1 = x11;
  //int8_t x0 = x00;
  uint16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  uint16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// draw a rectangle
void RGBmatrixPanelDue::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint16_t color) {
  drawLine(x, y, x+w-1, y, color);
  drawLine(x, y+h-1, x+w-1, y+h-1, color);

  drawLine(x, y, x, y+h-1, color);
  drawLine(x+w-1, y, x+w-1, y+h-1, color);
}

// fill a rectangle
void RGBmatrixPanelDue::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint16_t color) {
  for (uint8_t i=x; i<x+w; i++) {
    for (uint8_t j=y; j<y+h; j++) {
      drawPixel(i, j, color);
    }
  }
}



// draw a circle outline
void RGBmatrixPanelDue::drawCircle(uint8_t x0, uint8_t y0, uint8_t r, 
			uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
    
  }
}


// fill a circle
void RGBmatrixPanelDue::fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawLine(x0, y0-r, x0, y0+r+1, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawLine(x0+x, y0-y, x0+x, y0+y+1, color);
    drawLine(x0-x, y0-y, x0-x, y0+y+1, color);
    drawLine(x0+y, y0-x, x0+y, y0+x+1, color);
    drawLine(x0-y, y0-x, x0-y, y0+x+1, color);
  }
}

void RGBmatrixPanelDue::fill(uint16_t c) {
  for (uint8_t i=0; i<WIDTH; i++) {
    for (uint8_t j=0; j<HEIGHT; j++) {
      drawPixel(i, j, c);
    }
  }
}

void RGBmatrixPanelDue::setCursor(uint8_t x, uint8_t y) {
  cursor_x = x; 
  cursor_y = y;
}

void RGBmatrixPanelDue::setTextSize(uint8_t s) {
  textsize = s;
}

void RGBmatrixPanelDue::setTextColor(uint16_t c) {
  textcolor = c;
}

void RGBmatrixPanelDue::writeAChar(uint8_t c) {
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(cursor_x, cursor_y, c, textcolor, textsize);
    cursor_x += textsize*6;
  }
}


// draw a character
void RGBmatrixPanelDue::drawChar(uint8_t x, uint8_t y, char c, 
			      uint16_t color, uint8_t size) {
  for (uint8_t i =0; i<5; i++ ) {
    //uint8_t line = pgm_read_byte(font+(c*5)+i);
    uint8_t line = font[c*5+i];
    for (uint8_t j = 0; j<8; j++) {
      if (line & 0x1) {
	if (size == 1) // default size
	  drawPixel(x+i, y+j, color);
	else {  // big size
	  fillRect(x+i*size, y+j*size, size, size, color);
	} 
      }
      line >>= 1;
    }
  }
}


void RGBmatrixPanelDue::dumpMatrix(void) {
  uint8_t i=0;
  
  do {
    Serial.print("0x");
    if (matrixbuff[i] < 0xF)  Serial.print('0');
    Serial.print(matrixbuff[i], HEX);
    Serial.print(" ");
    i++;
    if (! (i %32) ) Serial.println();
  } while (i != 0);
  
    
}



void RGBmatrixPanelDue::writeSection(uint8_t secn, uint8_t *buffptr) {

  

  //Serial.println('here!');
   //digitalWrite(OE, HIGH);
  uint16_t portCstatus_nonclk = 0x0010; // CLK = low
  uint16_t portCstatus = 0x0010; // OE = HIGH
  uint16_t oeLow = 0x0020;
  portCstatus |= 0x0020; // clk is high here too
  REG_PIOC_ODSR = portCstatus; // set OE, CLK to high
    
  // set A, B, C pins
  if (secn & 0x1){  // Apin
    portCstatus |= 0x0002;
    portCstatus_nonclk |= 0x0002;
    oeLow |= 0x0002;
  }
  if (secn & 0x2){ // Bpin
    portCstatus |= 0x0004;
    portCstatus_nonclk |= 0x0004;
    oeLow |= 0x0004;
  } 
  if (secn & 0x4){ // Cpin
    portCstatus |= 0x0008;
    portCstatus_nonclk |= 0x0008;
    oeLow |= 0x0008;
   
  }
  REG_PIOC_ODSR = portCstatus; // set A, B, C pins
 
  uint8_t  low, high;
  uint16_t out = 0x0000;
  
  uint8_t i;
  for ( i=0; i<single_matrix_width*NX*NY; i++) {

    out = 0x0000;
   
    // red
   low = *buffptr++;
   high = low >> 4;
   low &= 0x0F;
   if (low > pwmcounter) out |= 0x0200; // R2, pin 30, PD9
   if (high > pwmcounter) out |= 0x0400; // R1, pin 32, PD10

   // green
   low = *buffptr++;
   high = low >> 4;
   low &= 0x0F;
   if (low > pwmcounter) out |= 0x0008; // G2, pin 28, PD3
   if (high > pwmcounter) out |= 0x0040; // G1, pin 29, PD6

   // blue
   low = *buffptr++;
   high = low >> 4;
   low &= 0x0F;
   if (low > pwmcounter) out |= 0x0002; // B2, pin 26, PD1
   if (high > pwmcounter) out |= 0x0004; // B1, pin 27, PD2
   
    
   //digitalWrite(CLK, LOW);
   REG_PIOC_ODSR = portCstatus_nonclk; // set clock to low, OE, A, B, C stay the same

   REG_PIOD_ODSR = out;

   //digitalWrite(CLK, HIGH);
   REG_PIOC_ODSR = portCstatus; // set clock to high, OE, A, B, C stay the same
   
  } 

  // latch it!
  
  //digitalWrite(LAT, HIGH);
  REG_PIOC_ODSR = (portCstatus |= 0x0040);

  //digitalWrite(LAT, LOW);  
  REG_PIOC_ODSR = portCstatus;
  
  //digitalWrite(OE, LOW);
  REG_PIOC_ODSR = oeLow; //portCstatus; //<< portCstatus;
}



void  RGBmatrixPanelDue::updateDisplay(void) {
  writeSection(scansection, matrixbuff + (PWMBITS*single_matrix_width*NX*NY*scansection));  
  scansection++;
  if (scansection == 8) { 
    scansection = 0;
    pwmcounter++;
    if (pwmcounter == PWMMAX) { pwmcounter = 0; }
  }
  //Serial.println(scansection);
}

void  RGBmatrixPanelDue::startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
        pmc_set_writeprotect(false);
        pmc_enable_periph_clk((uint32_t)irq);
        TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1);
        uint32_t rc = VARIANT_MCK/2/frequency; //2 because we selected TIMER_CLOCK1 above
        //TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
        //// VARIANT_MCK = 84000000, I guess timer4 is every 1/4 or something?
        //uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
        TC_SetRA(tc, channel, rc/2); //50% high, 50% low
        TC_SetRC(tc, channel, rc);
        TC_Start(tc, channel);
        tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
        tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
        NVIC_EnableIRQ(irq);
}
