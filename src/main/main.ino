#include <Wire.h>
#include "Configuration.h"

void setup() {
  Serial.begin(115200);
  
  // Initialize Motor Pins using defines from Configuration.h
  pinMode(PIN_AIN1, OUTPUT); 
  pinMode(PIN_AIN2, OUTPUT); 
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT); 
  pinMode(PIN_BIN2, OUTPUT); 
  pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);
  
  // Enable the motor driver
  digitalWrite(PIN_STBY, HIGH); 

  Serial.println("Micromouse System Ready...");
}

// Optimized Filtered Reading for Sharp GP2Y0A41SK0F
float getDistance(int pin) {
  long readingSum = 0;
  for(int i = 0; i < SENSOR_SAMPLES; i++) {
    readingSum += analogRead(pin);
  }
  float avgADC = readingSum / (float)SENSOR_SAMPLES;
  
  // Voltage conversion (Pico 2 uses 3.3V logic)
  float voltage = avgADC * (3.3 / 1023.0);
  
  // If voltage is too low, distance is beyond sensor range
  if (voltage < 0.3) return 40.0; 
  
  // Standard power-law formula for Sharp sensors
  return 12.08 * pow(voltage, -1.058); 
}

void drive(int left, int right) {
  // Left Motor Control (Channel B)
  digitalWrite(PIN_BIN1, left >= 0 ? HIGH : LOW);
  digitalWrite(PIN_BIN2, left >= 0 ? LOW : HIGH);
  analogWrite(PIN_PWMB, constrain(abs(left), 0, 255));

  // Right Motor Control (Channel A)
  digitalWrite(PIN_AIN1, right >= 0 ? HIGH : LOW);
  digitalWrite(PIN_AIN2, right >= 0 ? LOW : HIGH);
  analogWrite(PIN_PWMA, constrain(abs(right), 0, 255));
}

void loop() {
  // Read filtered distances from sensors
  float frontDist = getDistance(SENSOR_FRONT_PIN);
  float leftDist  = getDistance(SENSOR_LEFT_PIN);
  float rightDist = getDistance(SENSOR_RIGHT_PIN);

  // DEBUG: Print values to Serial Monitor
  Serial.print("F: "); Serial.print(frontDist);
  Serial.print(" L: "); Serial.print(leftDist);
  Serial.print(" R: "); Serial.println(rightDist);

  // LOGIC: Simple Obstacle Avoidance
  if (frontDist < WALL_THRESHOLD) {
    drive(0, 0); // Stop
    delay(200);
    
    if (leftDist > rightDist) {
      // More room on the left, turn left
      drive(-TURN_SPEED, TURN_SPEED);
      delay(250); // Calibrate this delay for a 90-degree turn
    } else {
      // More room on the right, turn right
      drive(TURN_SPEED, -TURN_SPEED);
      delay(250); // Calibrate this delay for a 90-degree turn
    }
    drive(0, 0); // Stop after turn
    delay(100);
  } else {
    // Path is clear, drive straight
    drive(BASE_SPEED, BASE_SPEED);
  }

  delay(10); // Small loop delay
}