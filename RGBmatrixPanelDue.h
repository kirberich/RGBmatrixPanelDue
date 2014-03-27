// Controller for RGB matrix panels that use 1:8 refresh mode

#include <Arduino.h>

//#define PWMBITS 3      // 3 = 3 bit color per LED, 9-bit total pixel color
//#define PWMMAX ((1 << PWMBITS) - 1)

//#define WIDTH 32       // RGB LEDs
//#define HEIGHT 16      //  " 

//#if (PWMBITS == 3) || (PWMBITS == 4) 
//#define NUMBYTES (WIDTH * HEIGHT / 2) * 3  // use 1.5 bytes (12 bits) per pixel
//#endif 

#define single_matrix_height 16
#define single_matrix_width 32

// fixed pin structure for now
#define APIN     33
#define BPIN     34
#define CPIN     35
#define LAT   38
#define OE    36
#define CLK   37
#define R1    32
#define G1    29
#define B1    27
#define R2    30
#define G2    28
#define B2    26


#define swap(a, b) { uint16_t t = a; a = b; b = t; }

class RGBmatrixPanelDue {
 public:
  RGBmatrixPanelDue(uint8_t xpanels, uint8_t ypanels, uint8_t planes);
  void begin(uint32_t freq);
  uint16_t Color333(uint8_t r, uint8_t g, uint8_t b);
  uint16_t Color444(uint8_t r, uint8_t g, uint8_t b);
  uint16_t Color888(uint8_t r, uint8_t g, uint8_t b);
  void drawPixel(uint8_t x, uint8_t y, uint16_t c);
  void drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint16_t c);
  void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color); 
  void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
  void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
  void fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
  void fill(uint16_t c);

  // Printing
  void setCursor(uint8_t x, uint8_t y);
  void setTextSize(uint8_t s);
  void setTextColor(uint16_t c);
  void writeAChar(uint8_t c);
  void drawChar(uint8_t x, uint8_t y, char c, uint16_t color, uint8_t size);

  void updateDisplay();
  void dumpMatrix(void);

  uint8_t width();
  uint8_t height();
  void startTimer(Tc *tc, uint32_t ch, IRQn_Type IRQ, uint32_t freq);

  uint8_t NX, NY, PWMMAX, PWMBITS;

 private:

  uint8_t WIDTH, HEIGHT;//, PWMMAX, PWMBITS;
  uint32_t NUMBYTES;

  //uint8_t matrixbuff[NUMBYTES];  // 768 bytes for 16x32
  uint8_t *matrixbuff;
  //uint8_t matrixbuff[768];

  uint8_t cursor_x, cursor_y, textsize;
  uint16_t textcolor;
  //uint8_t maxScan = 8;

  volatile uint8_t pwmcounter;
  volatile uint8_t scansection;

  void writeSection(uint8_t section, uint8_t *buffptr);

  //void TC3_Handler();

  //  uint8_t _a, _b, _c, _latch, _oe, _sclk;
  // volatile uint8_t *sclkportreg, *latportreg, *oeportreg;
  //volatile uint8_t *addraportreg, *addrbportreg, *addrcportreg;
  //uint8_t sclkpin, latpin, oepin, addrapin, addrbpin, addrcpin;
};
