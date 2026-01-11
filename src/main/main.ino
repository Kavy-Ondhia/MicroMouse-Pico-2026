#include <Wire.h>
#include "Configuration.h"
#include "Maze.h"

MazeManager myMaze;
int currentX = 0;
int currentY = 0;
int currentDir = NORTH; // Starting orientation 

void setup() {
  Serial.begin(115200);
  pinMode(PIN_AIN1, OUTPUT); pinMode(PIN_AIN2, OUTPUT); pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT); pinMode(PIN_BIN2, OUTPUT); pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  digitalWrite(PIN_STBY, HIGH); 

  Serial.println("System Ready. Press button to start...");
  while(digitalRead(PIN_BUTTON) == HIGH) { delay(10); }
  
  myMaze.floodFill(); 
}

float getDistance(int pin) {
  long readingSum = 0;
  for(int i = 0; i < SENSOR_SAMPLES; i++) readingSum += analogRead(pin);
  float voltage = (readingSum / (float)SENSOR_SAMPLES) * (3.3 / 1023.0);
  if (voltage < 0.3) return 40.0; 
  return 12.08 * pow(voltage, -1.058); 
}

void drive(int left, int right) {
  digitalWrite(PIN_BIN1, left >= 0 ? HIGH : LOW);
  digitalWrite(PIN_BIN2, left >= 0 ? LOW : HIGH);
  analogWrite(PIN_PWMB, constrain(abs(left), 0, 255));
  digitalWrite(PIN_AIN1, right >= 0 ? HIGH : LOW);
  digitalWrite(PIN_AIN2, right >= 0 ? LOW : HIGH);
  analogWrite(PIN_PWMA, constrain(abs(right), 0, 255));
}

void turn90(bool clockwise) {
  if(clockwise) drive(TURN_SPEED, -TURN_SPEED);
  else drive(-TURN_SPEED, TURN_SPEED);
  delay(280); // Adjust based on physical surface
  drive(0, 0);
  delay(100);
}

void loop() {
  // Update wall memory based on sensors
  float f = getDistance(SENSOR_FRONT_PIN);
  float l = getDistance(SENSOR_LEFT_PIN);
  float r = getDistance(SENSOR_RIGHT_PIN);

  if (f < WALL_THRESHOLD) myMaze.cells[currentX][currentY].walls[currentDir] = true;
  if (l < WALL_THRESHOLD) myMaze.cells[currentX][currentY].walls[(currentDir + 3) % 4] = true;
  if (r < WALL_THRESHOLD) myMaze.cells[currentX][currentY].walls[(currentDir + 1) % 4] = true;

  myMaze.floodFill();

  // Find neighbor with lowest potential
  int bestDir = currentDir;
  uint8_t minVal = 255;
  for (int i = 0; i < 4; i++) {
    if (!myMaze.cells[currentX][currentY].walls[i]) {
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

  // Execute movement
  if (bestDir != currentDir) {
    int turnDiff = (bestDir - currentDir + 4) % 4;
    if (turnDiff == 1) turn90(true);
    else if (turnDiff == 3) turn90(false);
    else if (turnDiff == 2) { turn90(true); turn90(true); }
    currentDir = bestDir;
  }

  drive(BASE_SPEED, BASE_SPEED);
  delay(500); // Travel time for 20cm unit square 
  drive(0, 0);

  if (currentDir == NORTH) currentY++; else if (currentDir == EAST) currentX++;
  else if (currentDir == SOUTH) currentY--; else if (currentDir == WEST) currentX--;

  if (myMaze.cells[currentX][currentY].distance == 0) {
    Serial.println("GOAL!");
    while(1) drive(0,0);
  }
}