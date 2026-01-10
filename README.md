# 🐭 MicroMouse-Pico-2026

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

## 🧠 Navigation Logic: Weighted Flood Fill
Unlike basic maze solvers that simply count steps, this robot uses a **Cost-Based Weighted Logic**.
- **Movement Cost:** Moving into an adjacent cell has a base cost of $10$.
- **Turn Penalty:** If the move requires a 90° turn, an additional penalty of $+20$ is added.
- **Result:** The robot doesn't just find the *shortest* path; it finds the *fastest* path with the fewest turns.

## 📂 Project Structure
```text
├── src/                # Main Arduino (.ino) code
├── lib/                # Custom drivers (MPU6050, SharpFilters)
├── hardware/           # Schematics and Component List (CSV)
├── docs/               # Rules and Maze specifications
└── README.md           # You are here
