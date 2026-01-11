#ifndef MAZE_H
#define MAZE_H

#include <Arduino.h>
#include "Configuration.h"

// Direction definitions for easier logic
#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

struct Cell {
    uint8_t distance;   // "Water level" (Potential value)
    bool walls[4];      // North, East, South, West (true = wall exists)
    bool visited;       // For exploration tracking
};

class MazeManager {
public:
    Cell cells[MAZE_SIZE][MAZE_SIZE];

    MazeManager() {
        initMaze();
    }

    // Phase 1: The Mental Map (Initialization)
    void initMaze() {
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {
                cells[x][y].distance = 255; // Set to "empty" high value
                cells[x][y].visited = false;
                for (int i = 0; i < 4; i++) cells[x][y].walls[i] = false;
                
                // Set boundary walls of the 16x16 maze
                if (y == MAZE_SIZE - 1) cells[x][y].walls[NORTH] = true;
                if (x == MAZE_SIZE - 1) cells[x][y].walls[EAST] = true;
                if (y == 0)             cells[x][y].walls[SOUTH] = true;
                if (x == 0)             cells[x][y].walls[WEST] = true;
            }
        }
    }

    // Phase 2: Dynamic Re-Flooding (Recalculate distances)
    void floodFill() {
        // 1. Reset all distances to 255
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) cells[x][y].distance = 255;
        }

        // 2. Set Goals to 0 (The center 4 cells)
        int goal = MAZE_SIZE / 2;
        cells[goal-1][goal-1].distance = 0;
        cells[goal][goal-1].distance = 0;
        cells[goal-1][goal].distance = 0;
        cells[goal][goal].distance = 0;

        // 3. Simple BFS (Water pouring out)
        bool changed = true;
        while (changed) {
            changed = false;
            for (int x = 0; x < MAZE_SIZE; x++) {
                for (int y = 0; y < MAZE_SIZE; y++) {
                    if (cells[x][y].distance == 255) continue;

                    // Check neighbors and update if path is not blocked by a wall
                    updateNeighbor(x, y, x, y + 1, NORTH, changed);
                    updateNeighbor(x, y, x + 1, y, EAST, changed);
                    updateNeighbor(x, y, x, y - 1, SOUTH, changed);
                    updateNeighbor(x, y, x - 1, y, WEST, changed);
                }
            }
        }
    }

private:
    void updateNeighbor(int x, int y, int nx, int ny, int dir, bool &changed) {
        // Stay within grid
        if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) return;
        
        // If there is no wall and the neighbor's distance can be improved
        if (!cells[x][y].walls[dir]) {
            if (cells[nx][ny].distance > cells[x][y].distance + 1) {
                cells[nx][ny].distance = cells[x][y].distance + 1;
                changed = true;
            }
        }
    }
};

#endif