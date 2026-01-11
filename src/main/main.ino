#include <Wire.h>
#include "Configuration.h"
#include "Maze.h"

MazeManager myMaze;
int currentX = 0;
int currentY = 0;
int currentDir = NORTH; 

void setup() {
  Serial.begin(115200);
  // ... (Your existing pinMode setup) ...
  pinMode(PIN_AIN1, OUTPUT); pinMode(PIN_AIN2, OUTPUT); pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT); pinMode(PIN_BIN2, OUTPUT); pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  digitalWrite(PIN_STBY, HIGH); 

  Serial.println("Ready. Build maze in Python first if testing logic!");
  while(digitalRead(PIN_BUTTON) == HIGH) { delay(10); }
  
  myMaze.floodFill(); 
}

// ... (Keep your getDistance, drive, and turn90 functions exactly as they are) ...

void loop() {
  // 1. SENSE: Update walls using the new setWall for symmetry
  float f = getDistance(SENSOR_FRONT_PIN);
  float l = getDistance(SENSOR_LEFT_PIN);
  float r = getDistance(SENSOR_RIGHT_PIN);

  if (f < WALL_THRESHOLD) myMaze.setWall(currentX, currentY, currentDir, true);
  if (l < WALL_THRESHOLD) myMaze.setWall(currentX, currentY, (currentDir + 3) % 4, true);
  if (r < WALL_THRESHOLD) myMaze.setWall(currentX, currentY, (currentDir + 1) % 4, true);

  // 2. THINK: Recalculate distances
  myMaze.floodFill();

  // 3. DECIDE: Use the helper function from our Maze.h
  int bestDir = myMaze.getBestDirection(currentX, currentY);

  // If no better direction found (shouldn't happen unless trapped), stay put
  if (bestDir == -1) {
    drive(0,0);
    return;
  }

  // 4. MOVE: Execute turns
  if (bestDir != currentDir) {
    int turnDiff = (bestDir - currentDir + 4) % 4;
    if (turnDiff == 1) turn90(true);       // Turn East (Right)
    else if (turnDiff == 3) turn90(false); // Turn West (Left)
    else if (turnDiff == 2) { turn90(true); turn90(true); } // U-Turn
    currentDir = bestDir;
  }

  // Forward movement
  drive(BASE_SPEED, BASE_SPEED);
  delay(500); // TRAVEL_TIME for one cell
  drive(0, 0);
  delay(100); // Brief pause for stability

  // Update internal coordinates
  if (currentDir == NORTH) currentY++; 
  else if (currentDir == EAST) currentX++;
  else if (currentDir == SOUTH) currentY--; 
  else if (currentDir == WEST) currentX--;

  // Check Goal
  if (myMaze.cells[currentX][currentY].distance == 0) {
    Serial.println("GOAL REACHED!");
    while(1) { drive(0,0); } // Stop forever
  }
}