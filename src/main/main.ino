#include <Wire.h>
#include "Configuration.h"

void setup() {
  Serial.begin(115200);
  
  // Initialize Motor Pins using defines from Configuration.h
  pinMode(PIN_AIN1, OUTPUT); pinMode(PIN_AIN2, OUTPUT); pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT); pinMode(PIN_BIN2, OUTPUT); pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);
  digitalWrite(PIN_STBY, HIGH); 

  Serial.println("Micromouse System Ready...");
}

// Optimized Filtered Reading
float getDistance(int pin) {
  long readingSum = 0;
  for(int i = 0; i < SENSOR_SAMPLES; i++) {
    readingSum += analogRead(pin);
  }
  float avgADC = readingSum / (float)SENSOR_SAMPLES;
  
  // Sharp GP2Y0A41SK0F voltage-to-distance conversion
  float voltage = avgADC * (3.3 / 1023.0);
  if (voltage < 0.4) return 40.0; // Max range
  return 12.08 * pow(voltage, -1.058); 
}

void drive(int left, int right) {
  // Left Motor Control
  digitalWrite(PIN_AIN1, left >= 0 ? HIGH : LOW);
  digitalWrite(PIN_AIN2, left >= 0 ? LOW : HIGH);
  analogWrite(PIN_PWMA, constrain(abs(left), 0, 255));

  // Right Motor Control
  digitalWrite(PIN_BIN1, right >= 0 ? HIGH : LOW);
  digitalWrite(PIN_BIN2, right >= 0 ? LOW : HIGH);
  analogWrite(PIN_PWMB, constrain(abs(right), 0, 255));
}

void loop() {
  float frontDist = getDistance(SENSOR_FRONT_PIN);
  float leftDist  = getDistance(SENSOR_LEFT_PIN);
  float rightDist = getDistance(SENSOR_RIGHT_PIN);

  // LOGIC: Simple Obstacle Avoidance for Testing
  if (frontDist < WALL_DISTANCE_THRESHOLD) {
    drive(0, 0); // Stop
    delay(200);
    
    if (leftDist > rightDist) {
      // Turn Left
      drive(-TURN_SPEED, TURN_SPEED);
      delay(300); // Adjust this delay for a perfect 90-degree turn
    } else {
      // Turn Right
      drive(TURN_SPEED, -TURN_SPEED);
      delay(300);
    }
  } else {
    // Drive Straight
    drive(BASE_SPEED, BASE_SPEED);
  }

  delay(20); 
}