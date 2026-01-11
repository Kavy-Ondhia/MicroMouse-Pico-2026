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
                
                // Outer wall logic 
                if (y == MAZE_SIZE - 1) cells[x][y].walls[NORTH] = true;
                if (x == MAZE_SIZE - 1) cells[x][y].walls[EAST] = true;
                if (y == 0)             cells[x][y].walls[SOUTH] = true;
                if (x == 0)             cells[x][y].walls[WEST] = true;
            }
        }
        // Rule: Start square has walls on 3 sides (West, South, East) 
        cells[0][0].walls[WEST] = true;
        cells[0][0].walls[SOUTH] = true;
        cells[0][0].walls[EAST] = true; 
    }

    void floodFill() {
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) cells[x][y].distance = 255;
        }

        // Rule: Target point is center (5,5) in 1-based indexing 
        // This corresponds to (4,4), (4,5), (5,4), (5,5) in 0-based code.
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