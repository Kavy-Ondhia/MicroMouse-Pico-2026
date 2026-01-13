#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// --- MOTOR CONTROL (TB6612FNG) ---
#define PIN_PWMB 11    // Left Speed
#define PIN_BIN1 12    // Left Direction 1
#define PIN_BIN2 13    // Left Direction 2
#define PIN_PWMA 16    // Right Speed
#define PIN_AIN1 17    // Right Direction 1
#define PIN_AIN2 18    // Right Direction 2
#define PIN_STBY 21    // Standby Pin

// --- ENCODERS ---
#define PIN_ENC_L_A 14
#define PIN_ENC_L_B 15
#define PIN_ENC_R_A 19
#define PIN_ENC_R_B 20

// --- I2C BUS (Shared SDA/SCL for MPU-6050 and Sensors) ---
#define PIN_SDA 4
#define PIN_SCL 5

// --- SENSORS (I2C XSHUT) ---
// UPDATED to match your physical mapping table (GP26, GP27, GP28)
#define SENSOR_FRONT_XSHUT 26 
#define SENSOR_LEFT_XSHUT  27 
#define SENSOR_RIGHT_XSHUT 28 

// Unique I2C addresses assigned during setup()
#define ADDR_FRONT 0x30
#define ADDR_LEFT  0x31
#define ADDR_RIGHT 0x32

// Placeholder IDs for software function calls
#define SENSOR_FRONT_ID 0
#define SENSOR_LEFT_ID  1
#define SENSOR_RIGHT_ID 2

// --- INTERFACE ---
#define PIN_BUTTON 22

// --- RULE-BASED CONSTANTS ---
#define MAZE_SIZE 10 
#define SENSOR_SAMPLES 20
#define WALL_THRESHOLD 120    // 120mm (Integer for TRON's FPU advice)

#define BASE_SPEED 150
#define TURN_SPEED 120
#define STRAIGHT_COST 10
#define TURN_COST 30

// Movement timing (estimated until encoder logic is finalized)
#define MOVE_TIME_PER_CELL 800 

#endif
