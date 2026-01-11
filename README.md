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
| **Power** | 7.4V 500mAh LiPo Battery |

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

## 🧠 Navigation Logic: Weighted Flood Fill
Unlike basic maze solvers that simply count steps, this robot uses a **Cost-Based Weighted Logic**.
- **Movement Cost:** Moving into an adjacent cell has a base cost of $10$.
- **Turn Penalty:** If the move requires a 90° turn, an additional penalty of $+20$ is added.
- **Result:** The robot doesn't just find the *shortest* path; it finds the *fastest* path with the fewest turns.

## 📂 Project Structure
* `/src`: Final Arduino (.ino) and configuration (.h) files.
* `/hardware`: Schematic diagrams and PCB layouts (In Progress).
* `/media`: Component photos and project progress shots.

---
*Created by [Kavy Ondhia](https://github.com/Kavy-Ondhia)*
