#include <Wire.h>
#include "Configuration.h"
#include "Maze.h"

// --- Global Objects ---
MazeManager myMaze;
int currentX = 0;
int currentY = 0;
int currentDir = NORTH; // 0:N, 1:E, 2:S, 3:W

void setup() {
  Serial.begin(115200);
  
  // Initialize Motor Pins
  pinMode(PIN_AIN1, OUTPUT); pinMode(PIN_AIN2, OUTPUT); pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT); pinMode(PIN_BIN2, OUTPUT); pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  
  digitalWrite(PIN_STBY, HIGH); 

  // Initialize I2C for MPU-6050 (Future use)
  Wire.setSDA(PIN_SDA);
  Wire.setSCL(PIN_SCL);
  Wire.begin();

  Serial.println("Micromouse Initialized. Press button to start...");
  
  // Wait for button press to start the maze exploration
  while(digitalRead(PIN_BUTTON) == HIGH) { delay(10); }
  
  myMaze.floodFill(); // Initial Phase 1: Mental Map
  Serial.println("Starting Exploration...");
}

// --- Sensor Logic ---
float getDistance(int pin) {
  long readingSum = 0;
  for(int i = 0; i < SENSOR_SAMPLES; i++) {
    readingSum += analogRead(pin);
  }
  float avgADC = readingSum / (float)SENSOR_SAMPLES;
  float voltage = avgADC * (3.3 / 1023.0);
  if (voltage < 0.3) return 40.0; 
  return 12.08 * pow(voltage, -1.058); 
}

// --- Movement Logic ---
void drive(int left, int right) {
  // Left Motor (B)
  digitalWrite(PIN_BIN1, left >= 0 ? HIGH : LOW);
  digitalWrite(PIN_BIN2, left >= 0 ? LOW : HIGH);
  analogWrite(PIN_PWMB, constrain(abs(left), 0, 255));
  // Right Motor (A)
  digitalWrite(PIN_AIN1, right >= 0 ? HIGH : LOW);
  digitalWrite(PIN_AIN2, right >= 0 ? LOW : HIGH);
  analogWrite(PIN_PWMA, constrain(abs(right), 0, 255));
}

void turn90(bool clockwise) {
  if(clockwise) drive(TURN_SPEED, -TURN_SPEED);
  else drive(-TURN_SPEED, TURN_SPEED);
  delay(280); // CALIBRATION NEEDED: Adjust for exact 90 deg
  drive(0, 0);
  delay(100);
}

// --- Navigation Logic (Phase 2: Updating the Map) ---
void updateWallsAndReFlood() {
  float f = getDistance(SENSOR_FRONT_PIN);
  float l = getDistance(SENSOR_LEFT_PIN);
  float r = getDistance(SENSOR_RIGHT_PIN);

  bool wallFound = false;

  // Front Wall
  if (f < WALL_THRESHOLD) {
    myMaze.cells[currentX][currentY].walls[currentDir] = true;
    wallFound = true;
  }
  // Left Wall
  if (l < WALL_THRESHOLD) {
    int leftDir = (currentDir + 3) % 4;
    myMaze.cells[currentX][currentY].walls[leftDir] = true;
    wallFound = true;
  }
  // Right Wall
  if (r < WALL_THRESHOLD) {
    int rightDir = (currentDir + 1) % 4;
    myMaze.cells[currentX][currentY].walls[rightDir] = true;
    wallFound = true;
  }

  if (wallFound) {
    myMaze.floodFill(); // Recalculate "Water Levels" if a wall is found
  }
}

void loop() {
  // 1. Scan and Update Map
  updateWallsAndReFlood();

  // 2. Decide Next Move (Move toward lowest distance value)
  uint8_t minVal = 255;
  int bestDir = currentDir;

  // Check 4 neighbors
  for (int i = 0; i < 4; i++) {
    if (!myMaze.cells[currentX][currentY].walls[i]) {
      // Check coordinates of neighbor
      int nx = currentX, ny = currentY;
      if (i == NORTH) ny++; else if (i == EAST) nx++; 
      else if (i == SOUTH) ny--; else if (i == WEST) nx--;

      if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
        if (myMaze.cells[nx][ny].distance < minVal) {
          minVal = myMaze.cells[nx][ny].distance;
          bestDir = i;
        }
      }
    }
  }

  // 3. Execute Move
  if (bestDir != currentDir) {
    // Determine if we need a 90 or 180 turn
    int turnDiff = (bestDir - currentDir + 4) % 4;
    if (turnDiff == 1) turn90(true);
    else if (turnDiff == 3) turn90(false);
    else if (turnDiff == 2) { turn90(true); turn90(true); }
    currentDir = bestDir;
  }

  // Drive forward into the next cell
  drive(BASE_SPEED, BASE_SPEED);
  delay(450); // CALIBRATION NEEDED: Time to travel exactly one cell (18cm)
  drive(0, 0);

  // Update virtual coordinates
  if (currentDir == NORTH) currentY++;
  else if (currentDir == EAST) currentX++;
  else if (currentDir == SOUTH) currentY--;
  else if (currentDir == WEST) currentX--;

  // Check if goal reached
  if (myMaze.cells[currentX][currentY].distance == 0) {
    Serial.println("GOAL REACHED!");
    while(1) { drive(0,0); } // Stop
  }

  delay(100);
}