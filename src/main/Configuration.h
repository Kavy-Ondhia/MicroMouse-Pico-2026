#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// --- MOTOR PINOUT (TB6612FNG) ---
#define PIN_AIN1 2
#define PIN_AIN2 3
#define PIN_PWMA 4   // Left Motor Speed
#define PIN_BIN1 5
#define PIN_BIN2 6
#define PIN_PWMB 7   // Right Motor Speed
#define PIN_STBY 8

// --- ANALOG SENSOR PINS (Sharp GP2Y0A) ---
#define SENSOR_LEFT_PIN  26
#define SENSOR_FRONT_PIN 27
#define SENSOR_RIGHT_PIN 28

// --- CALIBRATION CONSTANTS ---
#define WALL_DISTANCE_THRESHOLD 12.0  // Distance in cm to consider a wall "detected"
#define BASE_SPEED 180                // Speed for straight runs (0-255)
#define TURN_SPEED 150                // Speed for pivot turns
#define SENSOR_SAMPLES 15             // How many readings to average

// --- MAZE SETTINGS ---
#define MAZE_SIZE 16
#define GOAL_X 7                      // Center of the maze
#define GOAL_Y 7

#endif