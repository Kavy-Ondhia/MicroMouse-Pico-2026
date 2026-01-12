# MicroMouse-Pico-2026 🐭

An autonomous, high-performance maze-solving robot built on the **Raspberry Pi Pico 2** using the Arduino C++ framework. This project is designed for professional Micromouse competitions, focusing on high-speed navigation and intelligent pathfinding.

## 🚀 Key Features
- **Processor:** Powered by the RP2350 (Cortex-M33) for high-speed calculation.
- **Algorithm:** Weighted Flood Fill with **Priority Frontier** exploration.
- **Control System:** Dual-loop PID control for precise straight-line driving and 90-degree pivot turns.
- **Sensor Fusion:** Moving average filtering for Sharp GP2Y0A41SK0F analog sensors to handle infrared noise.
- **Simulation-Driven:** Custom Python tools to verify logic before hardware deployment.

## 🛠️ Hardware Specification
| Component | Details |
| :--- | :--- |
| **Microcontroller** | Raspberry Pi Pico 2 (RP2350) |
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

The robot implements an advanced **Weighted Flood Fill** algorithm. Unlike basic solvers that only look for the shortest geometric path, this system calculates the **fastest** path by accounting for physical constraints (acceleration vs. turning deceleration).

### **Phase 1: Initialization**
* **The Grid:** A 10x10 coordinate system. 
* **Goal Orientation:** Target cells (4,4), (4,5), (5,4), and (5,5) are assigned a potential value of `0`.
* **Initial Flood:** A Breadth-First Search (BFS) assigns distance-to-goal values assuming an empty maze.

### **Phase 2: Search & Live Mapping**
* **Dynamic Re-Flooding:** As sensors detect walls, the robot updates its internal map and immediately triggers a `floodFill()` to recalculate the potential field.
* **Reactive Decisions:** The robot moves toward the lowest neighbor value while scanning for new walls.

### **Phase 3: The Priority Frontier (Return Journey)**
During the return trip from the goal to the start, the robot utilizes a **Priority Frontier** logic to maximize map coverage:
1. **Priority 1:** Unvisited neighboring cells (to reveal potential straightaways).
2. **Priority 2:** Cells with the lowest distance value back to (0,0).
This ensures that the robot doesn't just find *a* path, but explores the maze enough to find the *best* path for the Speed Run.

### **Phase 4: Weighted Speed Optimization**
To solve the "Short vs. Fast" problem, the robot assigns **Cost Points**:
* **Straight Move:** 10 Points.
* **90-Degree Turn:** 30 Points (accounts for braking and pivot time).
* **180-Degree Turn:** 50 Points (accounts for braking and pivot time[Less than two 90-Degree turns]).
The robot will choose a longer straight path over a shorter path filled with turns if the total point cost is lower.



## 🎮 Simulation & Development Tools
We use a "Simulation-First" approach to ensure algorithm stability before hardware deployment.

### **Maze Builder (`custom_maze_maker.py`)**
A graphical utility to design and test custom maze layouts.
- **Toggle Walls:** Click on grid edges to add/remove walls.
- **Export:** Press `ENTER` to generate `set_wall()` code snippets for the simulator or robot.

### **Logic Simulator (`sim.py`)**
A Pygame-based environment that mimics the physical robot's state machine across four distinct phases:
1. `START_TO_END_1`: Initial discovery.
2. `GOAL_SCAN`: Mapping the center.
3. `END_TO_START_1`: Optimized return.
4. `RUN_2`: Applying the Priority Frontier to finalize the map.

## 🚀 How to Run
### **Simulation**
1. Install Pygame: `pip install pygame`
2. Run the builder: `python custom_maze_maker.py`
3. Export your layout and paste it into `sim.py`.
4. Run: `python sim.py`

### **Robot Deployment**
1. Open `main.ino` in the Arduino IDE.
2. Ensure you have the **Raspberry Pi Pico 2** board support installed (Earle Philhower core recommended).
3. Connect the robot and upload.

## 📂 Project Structure
* `/src`: Final Arduino (.ino) and configuration (.h) files.
* `/hardware`: Schematic diagrams and PCB layouts (In Progress).
* `/media`: Component photos and project progress shots.

---
*Created by [Kavy Ondhia](https://github.com/Kavy-Ondhia)*
