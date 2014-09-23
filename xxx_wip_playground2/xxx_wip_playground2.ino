#include "FastSPI_LED2.h"

#define SOP '<'
#define EOP '>'
#define NUM_LEDS 208
int BOTTOM_INDEX = 0;
int TOP_INDEX = int(NUM_LEDS/2);
int EVENODD = NUM_LEDS%2;

struct CRGB leds[NUM_LEDS];

int ledsX[NUM_LEDS][7]; //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, ETC)

//-PERISTENT VARS
int idex = 0;        //-LED INDEX (0 to NUM_LEDS-1
int idx_offset = 0;  //-OFFSET INDEX (BOTTOM LED TO ZERO WHEN LOOP IS TURNED/DOESN'T REALLY WORK)
int ihue = 0;        //-HUE (0-360)
int ibright = 0;     //-BRIGHTNESS (0-255)
int isat = 0;        //-SATURATION (0-255)
int bouncedirection = 0;  //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;      //-INC VAR FOR SIN LOOPS
int lcount = 0;      //-ANOTHER COUNTING VAR

//serial input detection
bool started = false;
bool ended = false;


//time keepers
unsigned long timeKeeper = 0;    // a millis() time-slice


//array for serial string data
byte index;

void setup()
{
//  Serial1.begin(9600);
//  //Serialbegin(9600);
  LEDS.setBrightness(24); // SET BRIGHTNESS TO 1/2 POWER
  LEDS.addLeds<LPD8806, 11, 13>(leds, NUM_LEDS);
//  sCmd.addCommand("m",   set_mode_strip);
//  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  
  one_color_all(0,0,0); //-BLANK STRIP
  
  LEDS.show();  
  //Serialprintln("---SETUP COMPLETE---");
}



void loop()
{
  timeKeeper = millis();
  demo_mode();
  
}
  
void demo_mode(){
  int r = 10;

  // requires ledX array

// yeahhhh

//  random_red();                            // cellular automata
//  for(int i=0; i<r*5; i++) {rule30(100);}

//  random_red();
  for(int i=0; i<r*20; i++) {random_burst(20);}
//for(int i=0; i<r*25; i++) {rainbow_fade(2);}
//for(int i=0; i<r*20; i++) {rainbow_loop(10, 20);}
//for(int i=0; i<r*8; i++) {random_march(20);} // yes!
//for(int i=0; i<r*8; i++) {random_march(40);}
//for(int i=0; i<r*15; i++) {rainbow_vertical(15, 50);}
//for(int i=0; i<r*3; i++) {flame();}
//one_color_all(0,0,0);


}
  
 //------------------------------------- UTILITY FXNS --------------------------------------

//-SET THE COLOR OF A SINGLE RGB LED
void set_color_led(int adex, int cred, int cgrn, int cblu) {  
  int bdex;
  
  if (idx_offset > 0) {  //-APPLY INDEX OFFSET 
    bdex = (adex + idx_offset) % NUM_LEDS;
  }
  else {bdex = adex;}

  leds[bdex].r = cred;
  leds[bdex].g = cgrn;
  leds[bdex].b = cblu;  
  }


//-FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
  //-ONLY WORKS WITH INDEX < TOPINDEX
  if (i == BOTTOM_INDEX) {return BOTTOM_INDEX;}
  if (i == TOP_INDEX && EVENODD == 1) {return TOP_INDEX + 1;}
  if (i == TOP_INDEX && EVENODD == 0) {return TOP_INDEX;}
  return NUM_LEDS - i;  
}

//-FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
  //int N2 = int(NUM_LEDS/2);
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {iN = ( i + TOP_INDEX ) % NUM_LEDS; }
  return iN;
}


//-FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
  int r;
  if (i < NUM_LEDS - 1) {r = i + 1;}
  else {r = 0;}
  return r;
}


//-FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {r = i - 1;}
  else {r = NUM_LEDS - 1;}
  return r;
}


//-CONVERT HSV VALUE TO RGB
void HSVtoRGB(int hue, int sat, int val, int colors[3]) {
	// hue: 0-359, sat: 0-255, val (lightness): 0-255
	int r, g, b, base;

	if (sat == 0) { // Achromatic color (gray).
		colors[0]=val;
		colors[1]=val;
		colors[2]=val;
	} else  {
		base = ((255 - sat) * val)>>8;
		switch(hue/60) {
			case 0:
				r = val;
				g = (((val-base)*hue)/60)+base;
				b = base;
				break;
			case 1:
				r = (((val-base)*(60-(hue%60)))/60)+base;
				g = val;
				b = base;
				break;
			case 2:
				r = base;
				g = val;
				b = (((val-base)*(hue%60))/60)+base;
				break;
			case 3:
				r = base;
				g = (((val-base)*(60-(hue%60)))/60)+base;
				b = val;
				break;
			case 4:
				r = (((val-base)*(hue%60))/60)+base;
				g = base;
				b = val;
				break;
			case 5:
				r = val;
				g = base;
				b = (((val-base)*(60-(hue%60)))/60)+base;
				break;
		}
		colors[0]=r;
		colors[1]=g;
		colors[2]=b;
	}
}


void copy_led_array(){
  for(int i = 0; i < NUM_LEDS; i++ ) {
    ledsX[i][0] = leds[i].r;
    ledsX[i][1] = leds[i].g;
    ledsX[i][2] = leds[i].b;
  }  
}


void print_led_arrays(int ilen){
  copy_led_array();
  //Serialprintln("~~~***ARRAYS|idx|led-r-g-b|ledX-0-1-2");
  for(int i = 0; i < ilen; i++ ) {
      //Serialprint("~~~");
      //Serialprint(i);
      //Serialprint("|");      
      //Serialprint(leds[i].r);
      //Serialprint("-");
      //Serialprint(leds[i].g);
      //Serialprint("-");
      //Serialprint(leds[i].b);
      //Serialprint("|");      
      //Serialprint(ledsX[i][0]);
      //Serialprint("-");
      //Serialprint(ledsX[i][1]);
      //Serialprint("-");
      //Serialprintln(ledsX[i][2]);
    }  
}

//------------------------LED EFFECT FUNCTIONS------------------------

void one_color_all(int cred, int cgrn, int cblu) { //-SET ALL LEDS TO ONE COLOR
    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      set_color_led(i, cred, cgrn, cblu);
      LEDS.show();       
      delay(1);
    }  
}

void one_color_allNOSHOW(int cred, int cgrn, int cblu) { //-SET ALL LEDS TO ONE COLOR
    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      set_color_led(i, cred, cgrn, cblu);

    }  
}


void rainbow_fade(int idelay) { //-FADE ALL LEDS THROUGH HSV RAINBOW
    ihue++;
    if (ihue >= 359) {ihue = 0;}
    int thisColor[3];
    HSVtoRGB(ihue, 255, 255, thisColor);
    for(int idex = 0 ; idex < NUM_LEDS; idex++ ) {
      set_color_led(idex,thisColor[0],thisColor[1],thisColor[2]); 
    }
    LEDS.show();    
    delay(idelay);
}


void rainbow_loop(int istep, int idelay) { //-LOOP HSV RAINBOW
  idex++;
  ihue = ihue + istep;
  int icolor[3];  

  if (idex >= NUM_LEDS) {idex = 0;}
  if (ihue >= 359) {ihue = 0;}

  HSVtoRGB(ihue, 255, 255, icolor);
  set_color_led(idex, icolor[0], icolor[1], icolor[2]);
  LEDS.show();
  delay(idelay);
}


void random_burst(int idelay) { //-RANDOM INDEX/COLOR
  int icolor[3];  
  
  idex = random(0,NUM_LEDS);
  ihue = random(0,359);

  HSVtoRGB(ihue, 255, 255, icolor);
  set_color_led(idex, icolor[0], icolor[1], icolor[2]);
  LEDS.show();  
  delay(idelay);
}


void color_bounce(int idelay) { //-BOUNCE COLOR (SINGLE LED)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == NUM_LEDS) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }  
  for(int i = 0; i < NUM_LEDS; i++ ) {
    if (i == idex) {set_color_led(i, 255, 0, 0);}
    else {set_color_led(i, 0, 0, 0);}
  }
  LEDS.show();
  delay(idelay);
}

void flicker(int thishue, int thissat) {
  int random_bright = random(0,255);
  int random_delay = random(10,100);
  int random_bool = random(0,random_bright);
  int thisColor[3];
  
  if (random_bool < 10) {
    HSVtoRGB(thishue, thissat, random_bright, thisColor);

    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      set_color_led(i, thisColor[0], thisColor[1], thisColor[2]);
    }
    
    LEDS.show();    
    delay(random_delay);
  }
}


void random_red() { //QUICK 'N DIRTY RANDOMIZE TO GET CELL AUTOMATA STARTED  
  int temprand;
  for(int i = 0; i < NUM_LEDS; i++ ) {
    temprand = random(0,100);
    if (temprand > 50) {leds[i].r = 255;}
    if (temprand <= 50) {leds[i].r = 0;}
    leds[i].b = 0; leds[i].g = 0;
  }
  LEDS.show();  
}


void rule30(int idelay) { //1D CELLULAR AUTOMATA - RULE 30 (RED FOR NOW)
  copy_led_array();
  int iCW;
  int iCCW;
  int y = 100;  
  for(int i = 0; i < NUM_LEDS; i++ ) {
    iCW = adjacent_cw(i);
    iCCW = adjacent_ccw(i);
    if (ledsX[iCCW][0] > y && ledsX[i][0] > y && ledsX[iCW][0] > y) {leds[i].r = 0;}
    if (ledsX[iCCW][0] > y && ledsX[i][0] > y && ledsX[iCW][0] <= y) {leds[i].r = 0;}
    if (ledsX[iCCW][0] > y && ledsX[i][0] <= y && ledsX[iCW][0] > y) {leds[i].r = 0;}
    if (ledsX[iCCW][0] > y && ledsX[i][0] <= y && ledsX[iCW][0] <= y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] > y && ledsX[iCW][0] > y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] > y && ledsX[iCW][0] <= y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] <= y && ledsX[iCW][0] > y) {leds[i].r = 255;}
    if (ledsX[iCCW][0] <= y && ledsX[i][0] <= y && ledsX[iCW][0] <= y) {leds[i].r = 0;}
  }
  
  LEDS.show();  
  delay(idelay);
}

void random_march(int idelay) { //RANDOM MARCH CCW
  copy_led_array();
  int iCCW;
  
  int acolor[3];
  HSVtoRGB(random(0,360), 255, 255, acolor);
  
  leds[0].r = acolor[0];
  leds[0].g = acolor[1];
  leds[0].b = acolor[2];
  
  for(int i = 1; i < NUM_LEDS ; i++ ) {  //-GET/SET EACH LED COLOR FROM CCW LED
    iCCW = adjacent_ccw(i);
    leds[i].r = ledsX[iCCW][0];
    leds[i].g = ledsX[iCCW][1];
    leds[i].b = ledsX[iCCW][2];    
  }
  
  LEDS.show();  
  delay(idelay);
}


void pop_horizontal(int ahue, int idelay) {  //-POP FROM LEFT TO RIGHT UP THE RING
  int acolor[3];
  HSVtoRGB(ahue, 255, 255, acolor);
  
  int ix;
  
  if (bouncedirection == 0) {
    bouncedirection = 1;
    ix = idex;  
  }
  else if (bouncedirection == 1) {
    bouncedirection = 0;
    ix = horizontal_index(idex);
    idex++;
    if (idex > TOP_INDEX) {idex = 0;}      
  }
  
  for(int i = 0; i < NUM_LEDS; i++ ) {
    if (i == ix) {leds[i].r = acolor[0]; leds[i].g = acolor[1]; leds[i].b = acolor[2];}
    else {leds[i].r = 0; leds[i].g = 0; leds[i].b = 0;}
  }
  
  LEDS.show();  
  delay(idelay);    
}


void flame() {
  int acolor[3];
  int idelay = random(0,35);
  
  float hmin = 0.1; float hmax = 45.0;
  float hdif = hmax-hmin;
  int randtemp = random(0,3);
  float hinc = (hdif/float(TOP_INDEX))+randtemp;

  int ahue = hmin;
  for(int i = 0; i < TOP_INDEX; i++ ) {
    
    ahue = ahue + hinc;

    HSVtoRGB(ahue, 255, 255, acolor);
    
    leds[i].r = acolor[0]; leds[i].g = acolor[1]; leds[i].b = acolor[2];
    int ih = horizontal_index(i);
    leds[ih].r = acolor[0]; leds[ih].g = acolor[1]; leds[ih].b = acolor[2];
    
    leds[TOP_INDEX].r = 255; leds[TOP_INDEX].g = 255; leds[TOP_INDEX].b = 255;
  
    LEDS.show();    
    delay(idelay);
  }
}

void rainbow_vertical(int istep, int idelay) { //-RAINBOW 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {idex = 0;}  
  ihue = ihue + istep;
  if (ihue > 359) {ihue = 0;}
  //Serialprintln(ihue);  
  int idexA = idex;
  int idexB = horizontal_index(idexA);

  int acolor[3];
  HSVtoRGB(ihue, 255, 255, acolor);

  set_color_led(idexA, acolor[0], acolor[1], acolor[2]);  
  set_color_led(idexB, acolor[0], acolor[1], acolor[2]);  
 
  LEDS.show();  
  delay(idelay);
}
