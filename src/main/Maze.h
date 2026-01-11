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
};

class MazeManager {
public:
    Cell cells[MAZE_SIZE][MAZE_SIZE];

    MazeManager() { initMaze(); }

    // Sets up the 10x10 grid with outer boundaries and starting cell walls
    void initMaze() {
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {
                cells[x][y].distance = 255; // Initialize as "infinity"
                for (int i = 0; i < 4; i++) cells[x][y].walls[i] = false;
                
                // Set outer boundary walls
                if (y == MAZE_SIZE - 1) setWall(x, y, NORTH, true);
                if (x == MAZE_SIZE - 1) setWall(x, y, EAST, true);
                if (y == 0)             setWall(x, y, SOUTH, true);
                if (x == 0)             setWall(x, y, WEST, true);
            }
        }
        // Competition starting cell (0,0) usually has West, South, and East walls
        setWall(0, 0, SOUTH, true);
        setWall(0, 0, WEST, true);
        setWall(0, 0, EAST, true);
    }

    // CRITICAL: Ensures when a wall is added to one cell, 
    // the neighbor cell is also updated (Symmetry).
    void setWall(int x, int y, int dir, bool present) {
        if (x < 0 || x >= MAZE_SIZE || y < 0 || y >= MAZE_SIZE) return;
        
        cells[x][y].walls[dir] = present;

        // Update the neighbor cell on the other side of the wall
        if (dir == NORTH && y < MAZE_SIZE - 1) cells[x][y + 1].walls[SOUTH] = present;
        if (dir == SOUTH && y > 0)             cells[x][y - 1].walls[NORTH] = present;
        if (dir == EAST  && x < MAZE_SIZE - 1) cells[x + 1][y].walls[WEST]  = present;
        if (dir == WEST  && x > 0)             cells[x - 1][y].walls[EAST]  = present;
    }

    // Returns the direction (0-3) that leads to the lowest distance value
    int getBestDirection(int x, int y) {
        uint8_t min_dist = cells[x][y].distance;
        int best_dir = -1;
        
        // N, E, S, W coordinate modifiers
        int dx[] = {0, 1, 0, -1};
        int dy[] = {1, 0, -1, 0};

        for (int i = 0; i < 4; i++) {
            // Only consider directions without a wall
            if (!cells[x][y].walls[i]) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
                    if (cells[nx][ny].distance < min_dist) {
                        min_dist = cells[nx][ny].distance;
                        best_dir = i;
                    }
                }
            }
        }
        return best_dir;
    }

    // Classic Flood Fill algorithm to calculate distances to the center
    void floodFill() {
        // Reset all distances to max
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) cells[x][y].distance = 255;
        }

        // Set the 4 center goal cells to distance 0
        cells[4][4].distance = 0; cells[4][5].distance = 0;
        cells[5][4].distance = 0; cells[5][5].distance = 0;

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
                        // If there is no wall and the neighbor's distance is higher than it should be
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