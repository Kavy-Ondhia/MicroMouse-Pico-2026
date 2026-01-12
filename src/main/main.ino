#include <Wire.h>
#include "Configuration.h"
#include "Maze.h"

enum RobotState { 
  SEARCHING_1,  // Run 1: Start to End
  RETURNING_1,  // Run 1: End to Start
  SEARCHING_2,  // Run 2: Start to End (Optimized Dry Run)
  RETURNING_2,  // Run 2: End to Start (Priority Frontier)
  READY_FOR_SPEED, 
  SPEED_RUN, 
  FINISHED 
};

RobotState currentState = SEARCHING_1;
MazeManager myMaze;
int currentX = 0, currentY = 0, currentDir = NORTH;

void setup() {
  Serial.begin(115200);
  // ... (Pin modes from your original code)
  digitalWrite(PIN_STBY, HIGH);
  Serial.println("Robot Ready. Press button...");
  while(digitalRead(PIN_BUTTON) == HIGH);
  myMaze.floodFill(4, 4); 
}

void loop() {
  if (currentState == FINISHED) { drive(0,0); return; }

  // 1. SENSE & REACTIVE UPDATE
  myMaze.markVisited(currentX, currentY);
  bool wallFound = false;
  
  float f = getDistance(SENSOR_FRONT_PIN);
  float l = getDistance(SENSOR_LEFT_PIN);
  float r = getDistance(SENSOR_RIGHT_PIN);

  if (f < WALL_THRESHOLD) { if(!myMaze.cells[currentX][currentY].walls[currentDir]) { myMaze.setWall(currentX, currentY, currentDir, true); wallFound = true; } }
  if (l < WALL_THRESHOLD) { if(!myMaze.cells[currentX][currentY].walls[(currentDir+3)%4]) { myMaze.setWall(currentX, currentY, (currentDir+3)%4, true); wallFound = true; } }
  if (r < WALL_THRESHOLD) { if(!myMaze.cells[currentX][currentY].walls[(currentDir+1)%4]) { myMaze.setWall(currentX, currentY, (currentDir+1)%4, true); wallFound = true; } }

  // 2. THINK: Recalculate if map changed
  if (wallFound) {
    if (currentState == RETURNING_1 || currentState == RETURNING_2) myMaze.floodFill(0, 0);
    else myMaze.floodFill(4, 4);
  }

  // 3. DECIDE: Use Priority Logic for the return of Run 2
  int bestDir;
  if (currentState == RETURNING_2) {
    bestDir = myMaze.getPriorityDirection(currentX, currentY, currentDir);
  } else {
    // Standard weighted logic for other phases
    bestDir = myMaze.getWeightedDirection(currentX, currentY, currentDir, (currentState == RETURNING_1));
  }

  if (bestDir == -1) { currentState = FINISHED; return; }

  // 4. MOVE
  if (bestDir != currentDir) {
    int turnDiff = (bestDir - currentDir + 4) % 4;
    if (turnDiff == 1) turn90(true);
    else if (turnDiff == 3) turn90(false);
    else if (turnDiff == 2) { turn90(true); turn90(true); }
    currentDir = bestDir;
  }

  // Forward movement
  drive(BASE_SPEED, BASE_SPEED);
  delay(500); 
  if (currentDir == NORTH) currentY++; 
  else if (currentDir == EAST) currentX++;
  else if (currentDir == SOUTH) currentY--; 
  else if (currentDir == WEST) currentX--;
  drive(0, 0);
  delay(100);

  // 5. STATE TRANSITIONS
  uint8_t dist = myMaze.cells[currentX][currentY].distance;
  if (dist == 0) {
    drive(0,0); delay(1000);
    if (currentState == SEARCHING_1) { currentState = RETURNING_1; myMaze.floodFill(0, 0); }
    else if (currentState == RETURNING_1) { currentState = SEARCHING_2; myMaze.floodFill(4, 4); }
    else if (currentState == SEARCHING_2) { currentState = RETURNING_2; myMaze.floodFill(0, 0); }
    else if (currentState == RETURNING_2) { currentState = READY_FOR_SPEED; }
  }
}