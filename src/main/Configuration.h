#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// =====================================================
// MOTOR DRIVER PINS (TB6612FNG)
// =====================================================
// Left Motor (Channel B)
#define PIN_PWMB 11    // KiCad: GP11
#define PIN_BIN1 12    // KiCad: GP12
#define PIN_BIN2 13    // KiCad: GP13

// Right Motor (Channel A)
#define PIN_PWMA 16    // KiCad: GP16
#define PIN_AIN1 17    // KiCad: GP17
#define PIN_AIN2 18    // KiCad: GP18

// Standby Pin
#define PIN_STBY 21    // KiCad: GP21

// =====================================================
// ENCODER PINS (N20 Motors)
// =====================================================
#define PIN_ENC_L_A 14 // Left Phase A (Interrupt)
#define PIN_ENC_L_B 15 // Left Phase B
#define PIN_ENC_R_A 19 // Right Phase A (Interrupt)
#define PIN_ENC_R_B 20 // Right Phase B

// =====================================================
// SENSOR PINS (Sharp GP2Y0A41SK0F)
// =====================================================
#define SENSOR_FRONT_PIN 26 // KiCad: GP26_A0
#define SENSOR_LEFT_PIN  27 // KiCad: GP27_A1
#define SENSOR_RIGHT_PIN 28 // KiCad: GP28_A2

// =====================================================
// I2C & INTERFACE
// =====================================================
#define PIN_SDA 4      // KiCad: GP4 (MPU-6050)
#define PIN_SCL 5      // KiCad: GP5 (MPU-6050)
#define PIN_BUTTON 22  // KiCad: GP22

// =====================================================
// CONSTANTS & CALIBRATION
// =====================================================
#define SENSOR_SAMPLES 20        // Moving average count for noise reduction
#define WALL_THRESHOLD 10.0      // Distance in cm to detect a wall
#define BASE_SPEED 150           // Default PWM speed (0-255)
#define TURN_SPEED 120           // Speed during pivot turns
#define MAZE_SIZE 16             // Standard 16x16 grid

// Logic costs for Weighted Flood Fill
#define STRAIGHT_COST 10
#define TURN_COST 30

#endif
