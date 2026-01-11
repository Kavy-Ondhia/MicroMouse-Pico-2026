#include <Wire.h>
#include "Configuration.h"
#include "Maze.h"

// Define the robot's current goal state
enum RobotState { SEARCHING, RETURNING, FINISHED };
RobotState currentState = SEARCHING;

MazeManager myMaze;
int currentX = 0;
int currentY = 0;
int currentDir = NORTH; 

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_AIN1, OUTPUT); pinMode(PIN_AIN2, OUTPUT); pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT); pinMode(PIN_BIN2, OUTPUT); pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  digitalWrite(PIN_STBY, HIGH); 

  Serial.println("Robot Ready. Press button to start Search Phase...");
  while(digitalRead(PIN_BUTTON) == HIGH) { delay(10); }
  
  // Initial flood fill targeting the center (4,4 / 5,5)
  myMaze.floodFill(4, 4); 
}

void loop() {
  if (currentState == FINISHED) {
    drive(0, 0);
    return;
  }

  // 1. SENSE: Mark current cell as visited and update walls
  myMaze.markVisited(currentX, currentY);

  float f = getDistance(SENSOR_FRONT_PIN);
  float l = getDistance(SENSOR_LEFT_PIN);
  float r = getDistance(SENSOR_RIGHT_PIN);

  if (f < WALL_THRESHOLD) myMaze.setWall(currentX, currentY, currentDir, true);
  if (l < WALL_THRESHOLD) myMaze.setWall(currentX, currentY, (currentDir + 3) % 4, true);
  if (r < WALL_THRESHOLD) myMaze.setWall(currentX, currentY, (currentDir + 1) % 4, true);

  // 2. THINK: Recalculate distances based on the current target
  if (currentState == SEARCHING) {
    myMaze.floodFill(4, 4); // Target Center
  } else if (currentState == RETURNING) {
    myMaze.floodFill(0, 0); // Target Home
  }

  // 3. DECIDE: Use exploration bias (true) only when RETURNING
  bool useExplorationBias = (currentState == RETURNING);
  int bestDir = myMaze.getBestDirection(currentX, currentY, useExplorationBias);

  if (bestDir == -1) {
    drive(0,0);
    return;
  }

  // 4. MOVE: Execute turns
  if (bestDir != currentDir) {
    int turnDiff = (bestDir - currentDir + 4) % 4;
    if (turnDiff == 1) turn90(true);       // Right
    else if (turnDiff == 3) turn90(false); // Left
    else if (turnDiff == 2) { turn90(true); turn90(true); } // U-Turn
    currentDir = bestDir;
  }

  // Move Forward one cell
  drive(BASE_SPEED, BASE_SPEED);
  delay(500); 
  drive(0, 0);
  delay(100); 

  // Update internal coordinates
  if (currentDir == NORTH) currentY++; 
  else if (currentDir == EAST) currentX++;
  else if (currentDir == SOUTH) currentY--; 
  else if (currentDir == WEST) currentX--;

  // 5. STATE TRANSITION: Check if we reached the current goal
  if (myMaze.cells[currentX][currentY].distance == 0) {
    if (currentState == SEARCHING) {
      Serial.println("GOAL REACHED! Switching to Return & Refine...");
      currentState = RETURNING;
      delay(2000); // Victory pause
    } 
    else if (currentState == RETURNING) {
      Serial.println("BACK AT START! Map refined.");
      currentState = FINISHED;
    }
  }
}