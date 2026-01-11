#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// --- MOTOR CONTROL (Pico GP Numbers) ---
#define PIN_PWMB 11    
#define PIN_BIN1 12    
#define PIN_BIN2 13    
#define PIN_PWMA 16    
#define PIN_AIN1 17    
#define PIN_AIN2 18    
#define PIN_STBY 21    

// --- ENCODERS ---
#define PIN_ENC_L_A 14
#define PIN_ENC_L_B 15
#define PIN_ENC_R_A 19
#define PIN_ENC_R_B 20

// --- SENSORS (ADC Pins) ---
#define SENSOR_FRONT_PIN 26
#define SENSOR_LEFT_PIN  27
#define SENSOR_RIGHT_PIN 28

// --- INTERFACE ---
#define PIN_SDA 4
#define PIN_SCL 5
#define PIN_BUTTON 22

// --- RULE-BASED CONSTANTS ---
#define MAZE_SIZE 10         // Rule: 10x10 unit squares
#define SENSOR_SAMPLES 20
#define WALL_THRESHOLD 12.0  // In cm
#define BASE_SPEED 150
#define TURN_SPEED 120
#define STRAIGHT_COST 10
#define TURN_COST 30

#endif