#include <LPD8806.h> // Obtained from https://github.com/adafruit/LPD8806
#include <SPI.h>     // Provides SPI communication
#include "barrier.h" // Defines `barrier_t` struct with `position` and `level`

#define DEBUG true
#define DEBUG_TRAIL true

// Trail and strip variables
const unsigned int
  ledCount = 208,
  stepInterval = 10;
const unsigned char
  trailLevels[] = {127, 106, 88, 72, 58, 46, 36, 27, 20, 14, 10, 6, 4, 2, 1, 0},
  trailLength = sizeof(trailLevels);
int trail[trailLength] = {};
LPD8806 strip = LPD8806(ledCount); // Hardware SPI. Pins 11 & 13 on Arduino Uno.

// Barrier variables
const unsigned int
  barrierChance = 40,
  barrierInterval = 550,
  maxBarriers = 20;
const unsigned char
  barrierLevels[] = {6, 22, 56, 127},
  barrierStartLevel = sizeof(barrierLevels);
barrier_t barriers[maxBarriers];

// Teleport variables
const unsigned int teleportInterval = 2700;
int teleport[] = {-1, -1};

void drawScene() {
  for (int i = trailLength; i-- > 0;)
    strip.setPixelColor(trail[i], trailLevels[i], 0, 0);
  for (int i = maxBarriers; i-- > 0;) {
    barrier_t bar = barriers[i];
    if (bar.level)
      strip.setPixelColor(bar.position, 0, 0, barrierLevels[bar.level - 1]);
  }
  if (teleport[0] != -1) {
    strip.setPixelColor(teleport[0], 0, 127, 0);
    strip.setPixelColor(teleport[1], 127, 0, 22);
  }
  strip.show();
}

bool checkCollision(int position, barrier_t* &barrier) {
  for (int i = maxBarriers; i-- > 0;)
    if (position == barriers[i].position and barriers[i].level)
      return (barrier = &barriers[i]);
  return false;
}

bool checkTeleportation(int position, int &newPosition) {
  if (position != teleport[0] && position != teleport[1])
    return false;
  newPosition = (position == teleport[0]) ? teleport[1] : teleport[0];
  teleport[0] = -1;
  teleport[1] = -1;
  return true;
}

bool getFreeBarrier(barrier_t* &barrier) {
  for (int i = maxBarriers; i-- > 0;)
    if (!barriers[i].level)
      return (barrier = &barriers[i]);
  return false;
}

int getFreeStripPosition() {
  barrier_t* barrier;
  int index = random(ledCount);
  while (checkCollision(index, barrier) ||
         index == trail[0] || index == teleport[0] || index == teleport[1])
    index = random(ledCount);
  return index;
}

void moveTrail(void) {
  barrier_t* barrier;
  static char dir = 1;
  int teleportLocation = 0;
  for (int i = trailLength; i-- > 1;)
    trail[i] = trail[i - 1];
  if (checkTeleportation(trail[0], teleportLocation)) {
    trail[0] = teleportLocation;
    #if DEBUG
    Serial.print("Teleported to ");
    Serial.println(teleportLocation);
    #endif
  } else {
    if (checkCollision((trail[0] + ledCount + dir) % ledCount, barrier)) {
      #if DEBUG
      Serial.print("Collided @ ");
      Serial.print(barrier->position);
      Serial.print(" [");
      Serial.print(barrier->level, DEC);
      Serial.println("]");
      #endif
      if (--barrier->level == 0) // Barrier integrity reduces at collision.
        strip.setPixelColor(barrier->position, 0); // Remove broken barrier.
      dir *= -1;                      // Reverse direction after collision
    }
    trail[0] = (trail[0] + ledCount + dir) % ledCount;
    #if DEBUG && DEBUG_TRAIL
    Serial.print("H");
    Serial.println(trail[0]);
    #endif
  }
}

void placeBarrier() {
  barrier_t* barrier;
  if (random(100) < barrierChance)
    if (getFreeBarrier(barrier)) {
      *barrier = {getFreeStripPosition(), barrierStartLevel};
      #if DEBUG
      Serial.print("Barrier @ ");
      Serial.println(barrier->position);
      #endif
    }
}

void placeTeleport() {
  if (teleport[0] < 0) {
    teleport[0] = getFreeStripPosition();
    teleport[1] = getFreeStripPosition();
    #if DEBUG
    Serial.print("Teleport @ ");
    Serial.print(teleport[0]);
    Serial.print(",");
    Serial.println(teleport[1]);
    #endif
  }
}

void setup() {
  randomSeed(analogRead(0));
  #if DEBUG
  Serial.begin(115200);
  Serial.println("[RedRunner]");
  #endif
  strip.begin();
}

void loop() {
  static long
    nextBarrier = 0,
    nextStep = 0,
    nextTeleport = teleportInterval;
  long currentMillis = millis();
  if (currentMillis >= nextStep) {
    nextStep = currentMillis + stepInterval;
    moveTrail();
  }
  if (currentMillis >= nextBarrier) {
    nextBarrier = currentMillis + barrierInterval;
    placeBarrier();
  }
  if (currentMillis >= nextTeleport) {
    nextTeleport = currentMillis + teleportInterval;
    placeTeleport();
  }
  drawScene();
}
