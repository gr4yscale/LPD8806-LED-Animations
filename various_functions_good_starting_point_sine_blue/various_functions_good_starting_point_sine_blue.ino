#include <LPD8806.h>
#include "SPI.h"

/*************************************************************************************/
// Control LPD8806-based RGB LED strip - New American Public Art : Culture Tap, 2013
/*************************************************************************************/

//-- LED strip com pins --//
int dataPin = 13;
int clockPin = 11;

//-- timer Vars --//
unsigned long timeStamp;
unsigned long stopTime;


int wireComLast = 99;   // counter for the number of button presses
int wireComLastTotal = 0;   // counter for the number of button presses total        
int lastWireCom = 0;     // previous state of the button
int wireCom = 0;      // current state of the button

//-- table for breath pattern light show --//
uint8_t LED_Breathe_Table[]  = {   80,  87,  95, 103, 112, 121, 131, 141, 151, 161, 172, 182, 192, 202, 211, 220,
    228, 236, 242, 247, 251, 254, 255, 255, 254, 251, 247, 242, 236, 228, 220, 211, 202, 192, 182, 172, 161, 151, 
    141, 131, 121, 112, 103,  95,  87,  80,  73,  66,  60,  55,  50,  45,  41,  38,  34,  31,  28,  26,  24,  22,  
     20,  18,  16,  14,  12,  10,   8,   6,   4,   2,   0,   0,   0,   0,   0,   0,   0,   0,   2,   4,   6,   8,  
     10,  12,  14,  16,  18,  20,  22,  24,  26,  28,  31,  34,  38,  41,  45,  50,  55,  60,  66,  73 };

#define BREATHE_TABLE_SIZE (sizeof(LED_Breathe_Table))
#define BREATHE_CYCLE    6000      /*breathe cycle in milliseconds*/
#define BREATHE_UPDATE    (BREATHE_CYCLE / BREATHE_TABLE_SIZE)

// The LED strips are 32 LEDs per meter but you can extend/cut the strip
int numLEDs = 208;
LPD8806 strip = LPD8806(numLEDs);

void setup() {
  
  Serial.begin(115200);   
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}

// function prototypes, do not remove these!
void colorChase(uint32_t c, uint8_t wait, uint8_t wireNum);
void colorWipe(uint32_t c, uint8_t wait, uint8_t wireNum);
void dither(uint32_t c, uint8_t wait, uint8_t wireNum);
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait, uint8_t wireNum);
void wave(uint32_t c, int cycles, uint8_t wait, uint8_t wireNum);
void wave2(uint32_t c, int cycles, uint8_t wait, uint8_t wireNum);
void rainbowCycle(uint8_t wait, uint8_t wireNum);
void uniformBreathe(uint8_t* breatheTable, uint8_t breatheTableSize, uint16_t updatePeriod, uint16_t r, uint16_t g, uint16_t b, uint8_t wireNum);
void sequencedBreathe(uint8_t* breatheTable, uint8_t breatheTableSize, uint16_t updatePeriod, uint16_t r, uint16_t g, uint16_t b, uint8_t wireNum);
uint32_t Wheel(uint16_t WheelPos);

void loop() {

  wave2(255, 100, 100, 1);
//  rainbowCycle(20,20);
//  uniformBreathe(LED_Breathe_Table, BREATHE_TABLE_SIZE, BREATHE_UPDATE, 127, 127, 0, 31);
  
  //runShows();
  
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/// --- Below is all the nitty gritty of each show program -- /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void uniformBreathe(uint8_t* breatheTable, uint8_t breatheTableSize, uint16_t updatePeriod, uint16_t r, uint16_t g, uint16_t b, uint8_t wireNum)
{
  int i;
  uint8_t breatheIndex = 0;
  uint8_t breatheRed;
  uint8_t breatheGrn;
  uint8_t breatheBlu;
  
  for (breatheIndex = 0; breatheIndex < breatheTableSize; breatheIndex++) {
    for (i=0; i < strip.numPixels(); i++) {
      breatheRed = (r * breatheTable[breatheIndex]) / 256;
      breatheGrn = (g * breatheTable[breatheIndex]) / 256;
      breatheBlu = (b * breatheTable[breatheIndex]) / 256;
      strip.setPixelColor(i, breatheRed, breatheGrn, breatheBlu);      
    }
    strip.show();   // write all the pixels out
    delay(updatePeriod);
  }
}

void sequencedBreathe(uint8_t* breatheTable, uint8_t breatheTableSize, uint16_t updatePeriod, uint16_t r, uint16_t g, uint16_t b, uint8_t wireNum)
{
  int i;
  uint8_t breatheIndex = 0;
  uint8_t breatheRed;
  uint8_t breatheGrn;
  uint8_t breatheBlu;
  uint8_t sequenceIndex;
  
  for (breatheIndex = 0; breatheIndex < breatheTableSize; breatheIndex++) {
    for (i=0; i < strip.numPixels(); i++) {
      sequenceIndex = (breatheIndex + (i*4)) % breatheTableSize;
      breatheRed = (r * breatheTable[sequenceIndex]) / 256;
      breatheGrn = (g * breatheTable[sequenceIndex]) / 256;
      breatheBlu = (b * breatheTable[sequenceIndex]) / 256;
      strip.setPixelColor(i, breatheRed, breatheGrn, breatheBlu);
   
      
    }
    strip.show();   // write all the pixels out
    delay(updatePeriod);
  }
}



// Cycle through the color wheel, equally spaced around the belt
void rainbowCycle(uint8_t wait, uint8_t wireNum) {
  uint16_t i, j;

  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {

      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel(((i * 384 / strip.numPixels()) + j) % 384));
    }
  
    strip.show();   // write all the pixels out
    

    
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait, uint8_t wireNum) {
  
  int i;

  for (i=0; i < strip.numPixels(); i++) {    
      strip.setPixelColor(i, c);
      strip.show();   
  }

}


// An "ordered dither" fills every pixel in a sequence that looks
// sparkly and almost random, but actually follows a specific order.
void dither(uint32_t c, uint8_t wait, uint8_t wireNum) {
  // Determine highest bit needed to represent pixel index
  int hiBit = 0;
  int n = strip.numPixels() - 1;
  for(int bit=1; bit < 0x8000; bit <<= 1) {
    
    if(n & bit) hiBit = bit;
  }

  int bit, reverse;
  for(int i=0; i<(hiBit << 1); i++) {
    // Reverse the bits in i to create ordered dither:
    reverse = 0;
    for(bit=1; bit <= hiBit; bit <<= 1) {
      reverse <<= 1;
      if(i & bit) reverse |= 1;
    }
    strip.setPixelColor(reverse, c);
    strip.show();
    
 
  }
  
  
}

// "Larson scanner" = Cylon/KITT bouncing light effect
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait, uint8_t wireNum) {
  
  int i, j, pos, dir;

  pos = 0;
  dir = 1;

  for(i=0; i<((strip.numPixels()-1) * 2); i++) {
    // Draw 5 pixels centered on pos.  setPixelColor() will clip
    // any pixels off the ends of the strip, no worries there.
    // we'll make the colors dimmer at the edges for a nice pulse
    // look
    
    strip.setPixelColor(pos - 1, strip.Color(r, g, b));
    strip.setPixelColor(pos - 3, strip.Color(r, g, b));
    strip.setPixelColor(pos - 4, strip.Color(r, g, b));
    strip.setPixelColor(pos, strip.Color(r, g, b));
    strip.setPixelColor(pos + 1, strip.Color(r, g, b));
    strip.setPixelColor(pos + 3, strip.Color(r, g, b));
    strip.setPixelColor(pos + 4, strip.Color(r, g, b));
 

    strip.show();
    
    // If we wanted to be sneaky we could erase just the tail end
    // pixel, but it's much easier just to erase the whole thing
    // and draw a new one next time.
    for(j=-12; j<= 12; j++) 
        strip.setPixelColor(pos+j, strip.Color(0,0,0));
    // Bounce off ends of strip
    pos += dir;
    if(pos < 0) {
      pos = 1;
      dir = -dir;
    } else if(pos >= strip.numPixels()) {
      pos = strip.numPixels() - 12;
      dir = -dir;
    }
  }
}

// Sine wave effect
#define PI 3.14159265
void wave(uint32_t c, int cycles, uint8_t wait, uint8_t wireNum) {
  
  
  float y;
  byte  r, g, b, r2, g2, b2;

  // Need to decompose color into its r, g, b elements
  g = (c >> 16) & 0x7f;
  r = (c >>  8) & 0x7f;
  b =  c        & 0x7f; 

  for(int x=0; x<(strip.numPixels()*5); x++)
  {
    
    for(int i=0; i<strip.numPixels(); i++) {

      y = sin(PI * (float)cycles * (float)(x + i) / (float)strip.numPixels());
      if(y >= 0.0) {
        // Peaks of sine wave are white
        y  = 1.0 - y; // Translate Y to 0.0 (top) to 1.0 (center)
        r2 = 127 - (byte)((float)(127 - r) * y);
        g2 = 127 - (byte)((float)(127 - g) * y);
        b2 = 127 - (byte)((float)(127 - b) * y);
      } else {
        // Troughs of sine wave are black
        y += 1.0; // Translate Y to 0.0 (bottom) to 1.0 (center)
        r2 = (byte)((float)r * y);
        g2 = (byte)((float)g * y);
        b2 = (byte)((float)b * y);
      }
      strip.setPixelColor(i, r2, g2, b2);
    }
    strip.show();
  
    
  }
}

// Sine wave effect
#define PI 3.14159265
void wave2(uint32_t c, int cycles, uint8_t wait, uint8_t wireNum) {
  
  
  
  float y;
  byte  r, g, b, r2, g2, b2;

  // Need to decompose color into its r, g, b elements
  g = (c >> 16) & 0x7f;
  r = (c >>  8) & 0x7f;
  b =  c        & 0x7f; 

  for(int x=0; x<(strip.numPixels()*5); x++)
  {
    
    for(int i=0; i<strip.numPixels(); i++) {

      y = sin(PI * (float)cycles * (float)(x + i) / (float)strip.numPixels());
      if(y >= 0.0) {
        // Peaks of sine wave are white
        y  = 1.0 - y; // Translate Y to 0.0 (top) to 1.0 (center)
        r2 = 0 - (byte)((float)(0 - r) * y);
        g2 = 0 - (byte)((float)(0 - g) * y);
        b2 = 127 - (byte)((float)(127 - b) * y);
      } else {
        // Troughs of sine wave are black
        y += 1.0; // Translate Y to 0.0 (bottom) to 1.0 (center)
        r2 = 50 - (byte)((float)(50 - r) * y);
        g2 = 127 - (byte)((float)(127 - g) * y);
        b2 = (byte)((float)b * y);
      }
      strip.setPixelColor(i, r2, g2, b2);
    }
    strip.show();

   

  }
}

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g - b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
  return(strip.Color(r,g,b));
}
