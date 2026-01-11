#ifndef MAZE_H
#define MAZE_H

#include <Arduino.h>
#include "Configuration.h"

// Direction definitions matching the Python Simulator
#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

struct Cell {
    uint8_t distance;
    bool walls[4];
    bool visited; 
};

class MazeManager {
public:
    Cell cells[MAZE_SIZE][MAZE_SIZE];

    MazeManager() { initMaze(); }

    // Sets up the grid with boundaries and the start cell configuration
    void initMaze() {
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {
                cells[x][y].distance = 255; // Initialize as "infinity"
                cells[x][y].visited = false;
                for (int i = 0; i < 4; i++) cells[x][y].walls[i] = false;
                
                // Set outer boundary walls
                if (y == MAZE_SIZE - 1) setWall(x, y, NORTH, true);
                if (x == MAZE_SIZE - 1) setWall(x, y, EAST, true);
                if (y == 0)             setWall(x, y, SOUTH, true);
                if (x == 0)             setWall(x, y, WEST, true);
            }
        }
        // Starting cell (0,0) usually has West, South, and East walls
        setWall(0, 0, SOUTH, true);
        setWall(0, 0, WEST, true);
        setWall(0, 0, EAST, true);
    }

    // Ensures wall symmetry between neighboring cells
    void setWall(int x, int y, int dir, bool present) {
        if (x < 0 || x >= MAZE_SIZE || y < 0 || y >= MAZE_SIZE) return;
        
        cells[x][y].walls[dir] = present;

        if (dir == NORTH && y < MAZE_SIZE - 1) cells[x][y + 1].walls[SOUTH] = present;
        if (dir == SOUTH && y > 0)             cells[x][y - 1].walls[NORTH] = present;
        if (dir == EAST  && x < MAZE_SIZE - 1) cells[x + 1][y].walls[WEST]  = present;
        if (dir == WEST  && x > 0)             cells[x - 1][y].walls[EAST]  = present;
    }

    // Call this whenever the robot enters a cell
    void markVisited(int x, int y) {
        if (x >= 0 && x < MAZE_SIZE && y >= 0 && y < MAZE_SIZE) {
            cells[x][y].visited = true;
        }
    }

    /**
     * Returns the direction (0-3) that leads to the best path.
     * @param isReturning If true, applies a discovery bonus to unvisited cells.
     */
    int getBestDirection(int x, int y, bool isReturning) {
        float best_score = 1000.0;
        int best_dir = -1;
        
        int dx[] = {0, 1, 0, -1};
        int dy[] = {1, 0, -1, 0};

        for (int i = 0; i < 4; i++) {
            // Only consider directions without a physical wall
            if (!cells[x][y].walls[i]) {
                int nx = x + dx[i];
                int ny = y + dy[i];

                if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
                    float current_score = (float)cells[nx][ny].distance;

                    // Apply Exploration Bias: subtract from score to make unvisited cells more attractive
                    if (isReturning && !cells[nx][ny].visited) {
                        current_score -= 3.0; // The "Curiosity" factor
                    }

                    if (current_score < best_score) {
                        best_score = current_score;
                        best_dir = i;
                    }
                }
            }
        }
        return best_dir;
    }

    /** * Calculates distances to a specific target cell using Flood Fill.
     * For Search Phase: floodFill(4, 5)
     * For Return Phase: floodFill(0, 0)
     */
    void floodFill(int targetX, int targetY) {
        // Reset all distances
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) cells[x][y].distance = 255;
        }

        // Set target destination to 0
        if (targetX == 4 || targetX == 5) { // Center 2x2 Goal
             cells[4][4].distance = 0; cells[4][5].distance = 0;
             cells[5][4].distance = 0; cells[5][5].distance = 0;
        } else {
             cells[targetX][targetY].distance = 0;
        }

        bool changed = true;
        while (changed) {
            changed = false;
            for (int x = 0; x < MAZE_SIZE; x++) {
                for (int y = 0; y < MAZE_SIZE; y++) {
                    if (cells[x][y].distance == 255) continue;
                    
                    int dx[] = {0, 1, 0, -1};
                    int dy[] = {1, 0, -1, 0};
                    
                    for (int i = 0; i < 4; i++) {
                        int nx = x + dx[i], ny = y + dy[i];
                        if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE && !cells[x][y].walls[i]) {
                            if (cells[nx][ny].distance > cells[x][y].distance + 1) {
                                cells[nx][ny].distance = cells[x][y].distance + 1;
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
};

#endif