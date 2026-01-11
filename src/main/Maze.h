#ifndef MAZE_H
#define MAZE_H

#include <Arduino.h>
#include "Configuration.h"

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

    void initMaze() {
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {
                cells[x][y].distance = 255;
                cells[x][y].visited = false;
                for (int i = 0; i < 4; i++) cells[x][y].walls[i] = false;
                
                // Outer boundaries
                if (y == MAZE_SIZE - 1) cells[x][y].walls[NORTH] = true;
                if (x == MAZE_SIZE - 1) cells[x][y].walls[EAST] = true;
                if (y == 0)             cells[x][y].walls[SOUTH] = true;
                if (x == 0)             cells[x][y].walls[WEST] = true;
            }
        }
        // Start square rule (3 walls)
        cells[0][0].walls[WEST] = true;
        cells[0][0].walls[SOUTH] = true;
        cells[0][0].walls[EAST] = true; 
    }

    // New helper to add walls correctly like the Python Sim
    void setWall(int x, int y, int dir, bool present) {
        if (x < 0 || x >= MAZE_SIZE || y < 0 || y >= MAZE_SIZE) return;
        
        cells[x][y].walls[dir] = present;

        // Symmetry: if cell (0,0) has NORTH wall, (0,1) must have SOUTH wall
        if (dir == NORTH && y < MAZE_SIZE - 1) cells[x][y+1].walls[SOUTH] = present;
        if (dir == SOUTH && y > 0)             cells[x][y-1].walls[NORTH] = present;
        if (dir == EAST  && x < MAZE_SIZE - 1) cells[x+1][y].walls[WEST]  = present;
        if (dir == WEST  && x > 0)             cells[x-1][y].walls[EAST]  = present;
    }

    // Logic for robot to decide which way to turn
    int getBestDirection(int x, int y) {
        uint8_t min_dist = cells[x][y].distance;
        int best_dir = -1;

        // Check all 4 directions
        int dx[] = {0, 1, 0, -1}; // N, E, S, W
        int dy[] = {1, 0, -1, 0};

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
                // If there's no wall and the neighbor is closer to goal
                if (!cells[x][y].walls[i] && cells[nx][ny].distance < min_dist) {
                    min_dist = cells[nx][ny].distance;
                    best_dir = i;
                }
            }
        }
        return best_dir;
    }

    void floodFill() {
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) cells[x][y].distance = 255;
        }

        // Center Goals
        cells[4][4].distance = 0;
        cells[4][5].distance = 0;
        cells[5][4].distance = 0;
        cells[5][5].distance = 0;

        bool changed = true;
        while (changed) {
            changed = false;
            for (int x = 0; x < MAZE_SIZE; x++) {
                for (int y = 0; y < MAZE_SIZE; y++) {
                    if (cells[x][y].distance == 255) continue;
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
        if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) return;
        if (!cells[x][y].walls[dir]) {
            if (cells[nx][ny].distance > cells[x][y].distance + 1) {
                cells[nx][ny].distance = cells[x][y].distance + 1;
                changed = true;
            }
        }
    }
};

#endif