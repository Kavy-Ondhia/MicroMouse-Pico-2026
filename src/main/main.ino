#include <Wire.h>
#include "Configuration.h"
#include "Maze.h"

// --- Robot State Machine ---
enum RobotState { 
  SEARCHING_1,      // Run 1: Start to End (Exploring)
  RETURNING_1,      // Run 1: End to Start (Mapping)
  SEARCHING_2,      // Run 2: Start to End (Using found paths)
  RETURNING_2,      // Run 2: End to Start (Priority Frontier)
  READY_FOR_SPEED, 
  SPEED_RUN, 
  FINISHED 
};

RobotState currentState = SEARCHING_1;
MazeManager myMaze;
int currentX = 0, currentY = 0, currentDir = NORTH;

void setup() {
  Serial.begin(115200);
  
  // Initialize Pins (Assumes PIN_STBY, PIN_BUTTON, etc. are in Configuration.h)
  pinMode(PIN_STBY, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  digitalWrite(PIN_STBY, HIGH);

  Serial.println("Robot Ready. Press button to start...");
  while(digitalRead(PIN_BUTTON) == HIGH);
  
  // Initial flood fill toward the center goal (4,4)
  myMaze.floodFill(4, 4); 
}

void loop() {
  if (currentState == FINISHED) { 
    drive(0, 0); 
    return; 
  }

  // --- 1. SENSE & REACTIVE UPDATE ---
  // Mark current cell as visited using the new bitmask helper
  myMaze.markVisited(currentX, currentY);
  bool wallFound = false;
  
  float f = getDistance(SENSOR_FRONT_PIN);
  float l = getDistance(SENSOR_LEFT_PIN);
  float r = getDistance(SENSOR_RIGHT_PIN);

  // Check Front Wall
  if (f < WALL_THRESHOLD) { 
    if(!myMaze.hasWall(currentX, currentY, currentDir)) { 
      myMaze.setWall(currentX, currentY, currentDir, true); 
      wallFound = true; 
    } 
  }
  // Check Left Wall (currentDir - 1)
  if (l < WALL_THRESHOLD) { 
    int leftDir = (currentDir + 3) % 4;
    if(!myMaze.hasWall(currentX, currentY, leftDir)) { 
      myMaze.setWall(currentX, currentY, leftDir, true); 
      wallFound = true; 
    } 
  }
  // Check Right Wall (currentDir + 1)
  if (r < WALL_THRESHOLD) { 
    int rightDir = (currentDir + 1) % 4;
    if(!myMaze.hasWall(currentX, currentY, rightDir)) { 
      myMaze.setWall(currentX, currentY, rightDir, true); 
      wallFound = true; 
    } 
  }

  // --- 2. THINK: Recalculate if map changed ---
  if (wallFound) {
    if (currentState == RETURNING_1 || currentState == RETURNING_2) {
      myMaze.floodFill(0, 0); // Goal is Start
    } else {
      myMaze.floodFill(4, 4); // Goal is Center
    }
  }

  // --- 3. DECIDE ---
  int bestDir;
  if (currentState == RETURNING_2) {
    // Use the optimized Priority logic you built for Run 2 return
    bestDir = myMaze.getPriorityDirection(currentX, currentY, currentDir);
  } else {
    // Use the weighted distance logic for exploration
    // Note: Ensure getWeightedDirection in Maze.h is updated to use hasWall()
    bestDir = myMaze.getWeightedDirection(currentX, currentY, currentDir, (currentState == RETURNING_1));
  }

  if (bestDir == -1) { 
    currentState = FINISHED; 
    return; 
  }

  // --- 4. MOVE ---
  // Handle Turning
  if (bestDir != currentDir) {
    int turnDiff = (bestDir - currentDir + 4) % 4;
    if (turnDiff == 1) turn90(true);       // Right Turn
    else if (turnDiff == 3) turn90(false); // Left Turn
    else if (turnDiff == 2) {              // 180 Turn
      turn90(true); 
      turn90(true); 
    }
    currentDir = bestDir;
  }

  // Drive Forward one cell
  drive(BASE_SPEED, BASE_SPEED);
  delay(MOVE_TIME_PER_CELL); // This should ideally be handled by encoders
  
  if (currentDir == NORTH) currentY++; 
  else if (currentDir == EAST)  currentX++;
  else if (currentDir == SOUTH) currentY--; 
  else if (currentDir == WEST)  currentX--;
  
  drive(0, 0); // Brief stop to stabilize sensors
  delay(100);

  // --- 5. STATE TRANSITIONS ---
  uint8_t dist = myMaze.cells[currentX][currentY].distance;
  
  if (dist == 0) { // We reached the current goal
    drive(0, 0); 
    delay(1000); // Victory pause
    
    if (currentState == SEARCHING_1) { 
      currentState = RETURNING_1; 
      myMaze.floodFill(0, 0); 
    }
    else if (currentState == RETURNING_1) { 
      currentState = SEARCHING_2; 
      myMaze.floodFill(4, 4); 
    }
    else if (currentState == SEARCHING_2) { 
      currentState = RETURNING_2; 
      myMaze.floodFill(0, 0); 
    }
    else if (currentState == RETURNING_2) { 
      currentState = READY_FOR_SPEED; 
      Serial.println("Map optimized. Ready for Speed Run!");
    }
  }
}