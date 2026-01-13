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

// --- SENSORS (I2C & XSHUT) ---
// Note: XSHUT pins allow us to set unique I2C addresses for each sensor
#define SENSOR_FRONT_XSHUT 6   // Connect to XSHUT on Front Sensor
#define SENSOR_LEFT_XSHUT  7   // Connect to XSHUT on Left Sensor
#define SENSOR_RIGHT_XSHUT 8   // Connect to XSHUT on Right Sensor

// These are arbitrary unique addresses we will assign during setup()
#define ADDR_FRONT 0x30
#define ADDR_LEFT  0x31
#define ADDR_RIGHT 0x32

// Placeholder IDs for the getDistance() function
#define SENSOR_FRONT_ID 0
#define SENSOR_LEFT_ID  1
#define SENSOR_RIGHT_ID 2

// --- INTERFACE ---
#define PIN_SDA 4
#define PIN_SCL 5
#define PIN_BUTTON 22

// --- RULE-BASED CONSTANTS ---
#define MAZE_SIZE 10 
#define SENSOR_SAMPLES 20

// TRON: Changed to integer (mm) for better embedded performance
#define WALL_THRESHOLD 120    // 120mm (equivalent to your 12.0cm)

#define BASE_SPEED 150
#define TURN_SPEED 120
#define STRAIGHT_COST 10
#define TURN_COST 30

// Movement timing (placeholders until encoders are tuned)
#define MOVE_TIME_PER_CELL 800 

#endif