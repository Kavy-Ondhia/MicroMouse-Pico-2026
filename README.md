# MicroMouse-Pico-2026 🐭

An autonomous, high-performance maze-solving robot built on the **Raspberry Pi Pico 2** using the Arduino C++ framework. This project is designed for professional Micromouse competitions, focusing on high-speed navigation and intelligent pathfinding.

## 🚀 Key Features
- **Processor:** Powered by the RP2350 (Cortex-M33) for high-speed calculation.
- **Algorithm:** Weighted Flood Fill (prioritizes straightaways over turns).
- **Control System:** Dual-loop PID control for precise straight-line driving and 90-degree pivot turns.
- **Sensor Fusion:** Moving average filtering for Sharp GP2Y0A41SK0F analog sensors to handle infrared noise.

## 🛠️ Hardware Specification
| Component | Details |
| :--- | :--- |
| **Microcontroller** | Raspberry Pi Pico 2 |
| **Motor Driver** | TB6612FNG (Dual Channel) |
| **Motors** | N20 6V 150RPM with Encoders |
| **Sensors** | 3x Sharp GP2Y0A41SK0F (Front, Left, Right) |
| **IMU** | MPU-6050 (Gyroscope for turn precision) |
| **Power** | 7.4V LiPo Battery regulated via **L7805 IC** (5V Logic) |

## 🔌 Connection Table
**Microcontroller:** Raspberry Pi Pico 2
**Power Source:** 7.4V LiPo (Regulated to 5V via L7805 IC for Logic)

| Standard Pin | KiCad Symbol Label | Component | Function |
| :--- | :--- | :--- | :--- |
| **Pin 39** | VSYS | L7805 IC (Pin 3) | **+5V Input** (System Power) |
| **Pin 38** | GND | Common GND | **Ground** |
| **Pin 6** | GP4 | MPU-6050 | SDA (I2C Data) |
| **Pin 7** | GP5 | MPU-6050 | SCL (I2C Clock) |
| **Pin 15** | GP11 | TB6612FNG | PWMB (Left Speed) |
| **Pin 16** | GP12 | TB6612FNG | BIN1 (Left Direction 1) |
| **Pin 17** | GP13 | TB6612FNG | BIN2 (Left Direction 2) |
| **Pin 19** | GP14 | N20 Encoder (L) | Phase A (Interrupt) |
| **Pin 20** | GP15 | N20 Encoder (L) | Phase B |
| **Pin 21** | GP16 | TB6612FNG | PWMA (Right Speed) |
| **Pin 22** | GP17 | TB6612FNG | AIN1 (Right Direction 1) |
| **Pin 24** | GP18 | TB6612FNG | AIN2 (Right Direction 2) |
| **Pin 25** | GP19 | N20 Encoder (R) | Phase A (Interrupt) |
| **Pin 26** | GP20 | N20 Encoder (R) | Phase B |
| **Pin 27** | GP21 | TB6612FNG | STBY (Standby Pin) |
| **Pin 29** | GP22 | Push Button | Start/Stop Input |
| **Pin 31** | GP26_A0 | Front Sensor | Analog Output (Vo) |
| **Pin 32** | GP27_A1 | Left Sensor | Analog Output (Vo) |
| **Pin 34** | GP28_A2 | Right Sensor | Analog Output (Vo) |

## 🧠 Advanced Navigation: Weighted Flood Fill

The robot implements an advanced **Weighted Flood Fill** algorithm. Unlike basic solvers that only look for the shortest geometric path, this system calculates the **fastest** path by accounting for the physical limitations of the robot (acceleration vs. turning deceleration).

### **Phase 1: The "Mental Map" (Initialization)**
* **The Grid:** A digital 10x10 coordinate system (20cm x 20cm unit squares).
* **Goal Orientation:** The target point is the center of the maze. The center cells (4,4), (4,5), (5,4), and (5,5) [using 0-based indexing] are assigned a potential value of `0`.
* **Initial Flood:** The algorithm performs a breadth-first search (BFS) from the center outward, assigning a "distance-to-goal" value to every cell assuming no walls exist.

### **Phase 2: The Search Phase (Exploration)**
* **Heuristic Movement:** The robot always moves toward the adjacent cell with the lowest potential value.
* **Live Mapping:** As the robot enters a cell, the three Sharp IR sensors (Front, Left, Right) scan for walls.
* **Dynamic Re-Flooding:** If a new wall is detected, it is stored in memory and the "Potential Field" (the numbers) is immediately recalculated. 
* **Safety Bias:** If two paths have the same weight, the logic prioritizes "Openness"—choosing the path that keeps the robot away from walls to reduce the risk of crashing.

### **Phase 3: The "Weighted" Calculation (Speed Optimization)**
To solve the "Short vs. Fast" problem, the robot assigns **Cost Points** to every potential move:
* **Straight Move:** 10 Points.
* **90-Degree Turn:** 30 Points (accounts for braking and pivot time).

**Example:**
* **Path A (Short):** 5 cells + 2 turns = **110 Points**.
* **Path B (Longer):** 7 cells + 0 turns = **70 Points**.
* **Decision:** The robot selects **Path B** because it is mathematically "cheaper" and physically faster.

### **Phase 4: Return & Refine (Mapping)**
After reaching the goal, the robot returns to (0,0). During the return trip, it intentionally explores "high-probability" areas of the maze to discover potential high-speed straightaways that were missed during the initial run.

### **Phase 5: The Speed Run (Execution)**
Once the fastest path is locked in:
* **Path Smoothing:** The discrete cell-to-cell moves are converted into continuous motion commands.
* **Acceleration Profiles:** The Pico 2 calculates S-curve acceleration and deceleration points.
* **Execution:** The robot follows the pre-calculated path at maximum velocity, using sensors only for "Wall Following" (micro-adjustments to stay centered) rather than navigation.

## 📂 Project Structure
* `/src`: Final Arduino (.ino) and configuration (.h) files.
* `/hardware`: Schematic diagrams and PCB layouts (In Progress).
* `/media`: Component photos and project progress shots.

---
*Created by [Kavy Ondhia](https://github.com/Kavy-Ondhia)*
