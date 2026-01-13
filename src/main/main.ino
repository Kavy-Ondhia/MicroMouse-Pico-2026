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

// Function Prototypes (Stubs for Hardware)
uint16_t getDistance(int sensorPin);
void drive(int leftSpeed, int rightSpeed);
void turn90(bool clockwise);

void setup() {
  Serial.begin(115200);
  
  // Initialize Pins
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
  myMaze.markVisited(currentX, currentY);
  bool wallFound = false;
  
  // TRON: Changed to uint16_t to avoid unnecessary float math
  uint16_t f = getDistance(SENSOR_FRONT_PIN);
  uint16_t l = getDistance(SENSOR_LEFT_PIN);
  uint16_t r = getDistance(SENSOR_RIGHT_PIN);

  // Check Front Wall
  if (f < WALL_THRESHOLD) { 
    if(!myMaze.hasWall(currentX, currentY, currentDir)) { 
      myMaze.setWall(currentX, currentY, currentDir, true); 
      wallFound = true; 
    } 
  }
  // Check Left Wall
  if (l < WALL_THRESHOLD) { 
    int leftDir = (currentDir + 3) % 4;
    if(!myMaze.hasWall(currentX, currentY, leftDir)) { 
      myMaze.setWall(currentX, currentY, leftDir, true); 
      wallFound = true; 
    } 
  }
  // Check Right Wall
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
      myMaze.floodFill(0, 0); 
    } else {
      myMaze.floodFill(4, 4); 
    }
  }

  // --- 3. DECIDE ---
  int bestDir;
  if (currentState == RETURNING_2) {
    bestDir = myMaze.getPriorityDirection(currentX, currentY, currentDir);
  } else {
    bestDir = myMaze.getWeightedDirection(currentX, currentY, currentDir, (currentState == RETURNING_1));
  }

  if (bestDir == -1) { 
    currentState = FINISHED; 
    return; 
  }

  // --- 4. MOVE ---
  if (bestDir != currentDir) {
    int turnDiff = (bestDir - currentDir + 4) % 4;
    if (turnDiff == 1) turn90(true);       
    else if (turnDiff == 3) turn90(false); 
    else if (turnDiff == 2) {              
      turn90(true); 
      turn90(true); 
    }
    currentDir = bestDir;
  }

  drive(BASE_SPEED, BASE_SPEED);
  delay(MOVE_TIME_PER_CELL); 
  
  if (currentDir == NORTH) currentY++; 
  else if (currentDir == EAST)  currentX++;
  else if (currentDir == SOUTH) currentY--; 
  else if (currentDir == WEST)  currentX--;
  
  drive(0, 0); 
  delay(100);

  // --- 5. STATE TRANSITIONS ---
  uint8_t dist = myMaze.cells[currentX][currentY].distance;
  
  if (dist == 0) { 
    drive(0, 0); 
    delay(1000); 
    
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
      Serial.println("Goal Reached! Ready for Speed Run.");
    }
  }
}

// --- HARDWARE ABSTRACTION LAYER (HAL) ---

uint16_t getDistance(int sensorPin) {
  // TODO: Implement VL53L0X I2C reading here once hardware arrives
  return 8190; // Default to 'no wall'
}

void drive(int leftSpeed, int rightSpeed) {
  // TODO: Implement PWM motor control
}

void turn90(bool clockwise) {
  // TODO: Implement gyro/encoder based turning
}