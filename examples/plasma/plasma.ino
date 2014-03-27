// plasma code based on Windell Oskay's plasma for Peggy 
// we sped it up a bunch with fixed point HSV->RGB conversion and
// sine table lookup but havent done fixed point sqrt. its a little sluggish :(
// LGPL

#include "RGBmatrixPanelDue.h"


// xpanels, ypanels, nplanes (tested w/3)
RGBmatrixPanelDue matrix(2, 3, 3);


typedef struct
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} ColorRGB;

//a color with 3 components: h, s and v
typedef struct 
{
  unsigned char h;
  unsigned char s;
  unsigned char v;
} ColorHSV;

unsigned char plasma[32][16];
long paletteShift;


//Converts an HSV color to RGB color w/fixed point 8.8
void HSVtoRGBFixed(void *vRGB, void *vHSV) 
{
  uint16_t r, g, b, h, s, v; //this function works with uint's between 0 and 255
  uint16_t f, p, q, t;

  ColorRGB *colorRGB=(ColorRGB *)vRGB;
  ColorHSV *colorHSV=(ColorHSV *)vHSV;

  h = colorHSV->h;
  s = colorHSV->s;
  v = colorHSV->v;

  //if saturation is 0, the color is a shade of grey
  if(s == 0) {
    b = v;
    g = b;
    r = g;
  }
  //if saturation > 0, more complex calculations are needed
  else
  {
    h *= 6; //to bring hue to a number between 0 and 6*255, better for the calculations
    f = h % 256;//the fractional part of h
    //Serial.print("\nf = "); Serial.print(f, DEC);
    //Serial.print(" s = "); Serial.println(s, DEC);
    p = (v * (255 - s)) / 256;
    q = (v * (255 - (s * f)/256)) / 256;
    t = (v * (255 - (s * (255 - f))/256)) / 256;
    //Serial.print("\tP = "); Serial.print(p, DEC); Serial.print(" Q = "); Serial.print(q, DEC); Serial.print(" T = "); Serial.println(t, DEC);
    switch(h/256)
    {
      case 0: r=v; g=t; b=p; break;
      case 1: r=q; g=v; b=p; break;
      case 2: r=p; g=v; b=t; break;
      case 3: r=p; g=q; b=v; break;
      case 4: r=t; g=p; b=v; break;
      case 5: r=v; g=p; b=q; break;
      default: r = g = b = 0; break;
    }
  }
  colorRGB->r = r;
  colorRGB->g = g;
  colorRGB->b = b;
  

}

float  dist(float a, float b, float c, float d) {
  float s =  sqrt((c-a)*(c-a)+(d-b)*(d-b));  
  return s;
}

const int16_t sinetab[512] = {
0, 3, 6, 9, 12, 15, 18, 21, 25, 28, 31, 34, 37, 40, 43, 46, 49, 53, 56, 59, 62,
65, 68, 71, 74, 77, 80, 83, 86, 89, 92, 95, 97, 100, 103, 106, 109, 112, 115, 117,
120, 123, 126, 128, 131, 134, 136, 139, 142, 144, 147, 149, 152, 155, 157, 159,
162, 164, 167, 169, 171, 174, 176, 178, 181, 183, 185, 187, 189, 191, 193, 195,
197, 199, 201, 203, 205, 207, 209, 211, 212, 214, 216, 217, 219, 221, 222, 224,
225, 227, 228, 230, 231, 232, 234, 235, 236, 237, 238, 239, 241, 242, 243, 244,
244, 245, 246, 247, 248, 249, 249, 250, 251, 251, 252, 252, 253, 253, 254, 254,
254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
254, 254, 254, 253, 253, 252, 252, 251, 251, 250, 249, 249, 248, 247, 246, 245,
244, 244, 243, 242, 241, 239, 238, 237, 236, 235, 234, 232, 231, 230, 228, 227,
225, 224, 222, 221, 219, 217, 216, 214, 212, 211, 209, 207, 205, 203, 201, 199,
197, 195, 193, 191, 189, 187, 185, 183, 181, 178, 176, 174, 171, 169, 167, 164,
162, 159, 157, 155, 152, 149, 147, 144, 142, 139, 136, 134, 131, 128, 126, 123,
120, 117, 115, 112, 109, 106, 103, 100, 97, 95, 92, 89, 86, 83, 80, 77, 74, 71,
68, 65, 62, 59, 56, 53, 49, 46, 43, 40, 37, 34, 31, 28, 25, 21, 18, 15, 12, 9, 6, 3, 
0, -3, -6, -9, -12, -15, -18, -21, -25, -28, -31, -34, -37, -40, -43, -46, -49, 
-53, -56, -59, -62, -65, -68, -71, -74, -77, -80, -83, -86, -89, -92, -95, -97,
-100, -103, -106, -109, -112, -115, -117, -120, -123, -126, -128, -131, -134, -136,
-139, -142, -144, -147, -149, -152, -155, -157, -159, -162, -164, -167, -169, -171,
-174, -176, -178, -181, -183, -185, -187, -189, -191, -193, -195, -197, -199, -201,
-203, -205, -207, -209, -211, -212, -214, -216, -217, -219, -221, -222, -224, -225,
-227, -228, -230, -231, -232, -234, -235, -236, -237, -238, -239, -241, -242, -243,
-244, -244, -245, -246, -247, -248, -249, -249, -250, -251, -251, -252, -252, -253,
-253, -254, -254, -254, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255,
-255, -255, -255, -255, -255, -254, -254, -254, -253, -253, -252, -252, -251, -251,
-250, -249, -249, -248, -247, -246, -245, -244, -244, -243, -242, -241, -239, -238,
-237, -236, -235, -234, -232, -231, -230, -228, -227, -225, -224, -222, -221, -219,
-217, -216, -214, -212, -211, -209, -207, -205, -203, -201, -199, -197, -195, -193,
-191, -189, -187, -185, -183, -181, -178, -176, -174, -171, -169, -167, -164, -162,
-159, -157, -155, -152, -149, -147, -144, -142, -139, -136, -134, -131, -128, -126,
-123, -120, -117, -115, -112, -109, -106, -103, -100, -97, -95, -92, -89, -86, -83,
-80, -77, -74, -71, -68, -65, -62, -59, -56, -53, -49, -46, -43, -40, -37, -34, -31,
-28, -25, -21, -18, -15, -12, -9, -6, -3 };

// returns 16 bit - 8.8 fixed point - this is still buggy for big numbers but
// its ok for this plasma, maybe come back and fix it later? :/
int16_t sine(double f) {

 uint32_t ff = f * 512 * 163; // 512 table entries, 163 to make division cleaner below

 //Serial.println(ff, DEC);
 ff /= 1024;   // divide by 2pi * 163
 //Serial.println(ff, DEC);
 ff %= 512;
 //Serial.print("indx #");
 //Serial.println(ff, DEC);
 //int16_t fixedr = (int16_t)pgm_read_word(sinetab+ff );
 int16_t fixedr = (int16_t)sinetab[ff];
 //Serial.println(fixedr);
 //fixedr /= 256;
 
 //Serial.println(fixedr);

 return fixedr;
}

void  plasma_morph()
{
  unsigned char x,y;
  int16_t value;
  ColorRGB colorRGB;
  ColorHSV colorHSV;

  for(y = 0; y < matrix.height(); y++)
    for(x = 0; x < matrix.width(); x++) {
      {
	value = sine(dist(x + paletteShift, y, 128, 128) / 8.0)
	  + sine(dist(x, y, 64, 64) / 8.0)
	  + sine(dist(x, y + paletteShift / 7, 192, 64) / 7.0)
	  + sine(dist(x, y, 192, 100) / 8.0);

	colorHSV.h=(unsigned char)((value) >> 1)&0xff;
	colorHSV.s=255; 
	colorHSV.v=255;

       /* Serial.print("H = "); Serial.print(colorHSV.h, DEC);	
        Serial.print(" S = "); Serial.print(colorHSV.s, DEC);	
        Serial.print(" V = "); Serial.print(colorHSV.v, DEC);	*/
	HSVtoRGBFixed(&colorRGB, &colorHSV);      // use a fixed point version of HSV to RGB conversion

/*        Serial.print("-> Rf = "); Serial.print(colorRGB.r, DEC);	
        Serial.print(" Gf = "); Serial.print(colorRGB.g, DEC);	
        Serial.print(" Bf = "); Serial.print(colorRGB.b, DEC);		
//HSVtoRGB(&colorRGB, &colorHSV);
        Serial.print("/ R = "); Serial.print(colorRGB.r, DEC);	
        Serial.print(" G = "); Serial.print(colorRGB.g, DEC);	
        Serial.print(" B = "); Serial.println(colorRGB.b, DEC);	
        */
	matrix.drawPixel(x, y, matrix.Color888(colorRGB.r, colorRGB.g, colorRGB.b)); 
      }
  }
  paletteShift++;
}

//TIMER: TC1 ch 0
void TC3_Handler()
{
  TC_GetStatus(TC1, 0);
  matrix.updateDisplay();
}



void setup() {
  Serial.begin(115200);

  // initialize the display
  matrix.begin(3000);
  
  // start with morphing plasma, but allow going to color cycling if desired.
  paletteShift=128000;
  unsigned char bcolor;

  //generate the plasma once
  for(unsigned char y = 0; y < matrix.height() ; y++)
    for(unsigned char x = 0; x < matrix.width(); x++)
    {
      //the plasma buffer is a sum of sines
      bcolor = (unsigned char)
      (
            128.0 + (128.0 * sin(x*8.0 / 16.0))
          + 128.0 + (128.0 * sin(y*8.0 / 16.0))
      ) / 2;
      plasma[x][y] = bcolor;
    }
    
   pinMode(12, OUTPUT);
   pinMode(13, OUTPUT);
}

void loop()
{
  plasma_morph();
}

