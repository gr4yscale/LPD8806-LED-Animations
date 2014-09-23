#include <LPD8806.h>
#include <SPI.h>
#include "ball.h"

const int
  ledCount = 208,
  ledPerMeter = 52,
  updateInterval = 1;
const float
  gravity = 5, // Earth gravity in m/s^2
//  gravity = 1.622, // Lunar gravity in m/s^2
  ledSpacing = 0.75 / ledPerMeter,
  stripCeiling = (ledCount - 1) * ledSpacing,
  timeFactor = updateInterval / 1000.0;
LPD8806
  strip = LPD8806(ledCount);
Ball balls[] = {
      Ball(stripCeiling, 25, .80, 1.0),
    Ball(stripCeiling, 30, .75, 1.4),
    Ball(stripCeiling, 15, .25, 1.6),
    Ball(stripCeiling, 50, .15, 1.9),
    Ball(stripCeiling, 15, .35, 1.11),
    Ball(stripCeiling, 9, .95, 1.6),
    Ball(stripCeiling, 25, .15, 1.22),
    Ball(stripCeiling, 11, .05, 1.8),
    Ball(stripCeiling, 150, .25, 0.4),
    Ball(stripCeiling, 40, .35, 1.1),
    Ball(stripCeiling, 25, .80, 1.0),
    Ball(stripCeiling, 1, .75, 1.4),
    Ball(stripCeiling, 25, .75, 1.2),
    Ball(stripCeiling, 50, .05, 1.9),
    Ball(stripCeiling, 15, .35, 2),
    Ball(stripCeiling, 9, .95, 1.6),
    Ball(stripCeiling, 25, .15, 1.22),
    Ball(stripCeiling, 11, .05, 1.8),
    Ball(stripCeiling, 200, .25, 2.4),
    Ball(stripCeiling, 40, .35, .8),
    Ball(stripCeiling, 1, .35, 1.0),
    Ball(stripCeiling, 20, .35, 1.1),
    Ball(stripCeiling, 30, .35, 0.3),
    Ball(stripCeiling, 40, .35, 0.2),
    
  };
    
const long colors[] = {
    strip.Color(127, 0, 100),
    strip.Color(0, 127, 50),
    strip.Color(10, 0, 127),
    strip.Color(100, 120, 3),
    strip.Color(40, 127, 40),
    strip.Color(10, 100, 0),
    strip.Color(90, 2, 27),
    strip.Color(127, 127, 127),
    strip.Color(60, 89, 7),
    strip.Color(2, 70, 70),
    strip.Color(127, 0, 100),
    strip.Color(0, 127, 50),
    strip.Color(10, 0, 127),
    strip.Color(100, 120, 3),
    strip.Color(40, 127, 40),
    strip.Color(10, 100, 0),
    strip.Color(90, 2, 27),
    strip.Color(127, 127, 127),
    strip.Color(60, 89, 7),
    strip.Color(2, 70, 70),
    strip.Color(12, 127, 127),
    strip.Color(120, 120, 7),
    strip.Color(2, 120, 30),
    
  };
const byte ballCount = sizeof(balls) / sizeof(Ball);

void setup() {
  strip.begin();
  strip.show();
//  Serial.begin(57600);
}

void loop() {
  static long nextStep = 0;
  long milliSeconds = millis();
//  if (Serial.available())
    serialAddKineticEnergy();
  if (milliSeconds >= nextStep) {
    nextStep += updateInterval;
    for (byte i = ballCount; i-- > 0;)
      balls[i].travel(timeFactor, gravity);
    renderDots();
  }
}

void serialAddKineticEnergy(void) {
//  byte peekChar = Serial.peek();
//  if (peekChar >= '0' && peekChar <= '9') {
    // Adds or subtracts kinetic energy to/from the current ball motion.
//    unsigned long addedForce = Serial.parseInt();
    unsigned long addedForce = 9;
    for (int i = 0; i < ballCount; ++i) {
//      Serial.print("Ball #");
//      Serial.print(i + 1);
//      Serial.print(": Force of ");
//      Serial.print(addedForce);
//      Serial.print("J changes speed from ");
//      Serial.print(balls[i].speed, 4);
//      Serial.print(" to ");
      balls[i].applyForce(addedForce);
//      Serial.print(balls[i].speed, 4);
//      Serial.println("m/s");
    }
//  } else {
//    if (Serial.read() == '?') {
//      for (int i = 0; i < ballCount; ++i) {
//        Serial.print("Ball #");
//        Serial.print(i + 1);
//        Serial.print(": ");
//        balls[i].serialReport();
//      }
//    }
//  }
}

void renderDots(void) {
  // Render the height of the ball on the line.
  for (int i = 0; i < ballCount; ++i) {
    int index = (balls[i].height + ledSpacing / 2) / ledSpacing;
    strip.setPixelColor(index, colors[random(0,24)] | strip.getPixelColor(index));
  }
  strip.show();
  for (int i = ballCount; i-- > 0;)
    strip.setPixelColor((balls[i].height + ledSpacing / 2) / ledSpacing, 0);
}
