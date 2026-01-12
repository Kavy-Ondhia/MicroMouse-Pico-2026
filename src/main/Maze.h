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
                
                if (y == MAZE_SIZE - 1) setWall(x, y, NORTH, true);
                if (x == MAZE_SIZE - 1) setWall(x, y, EAST, true);
                if (y == 0)             setWall(x, y, SOUTH, true);
                if (x == 0)             setWall(x, y, WEST, true);
            }
        }
    }

    void setWall(int x, int y, int dir, bool present) {
        if (x < 0 || x >= MAZE_SIZE || y < 0 || y >= MAZE_SIZE) return;
        cells[x][y].walls[dir] = present;
        if (dir == NORTH && y < MAZE_SIZE - 1) cells[x][y + 1].walls[SOUTH] = present;
        if (dir == SOUTH && y > 0)             cells[x][y - 1].walls[NORTH] = present;
        if (dir == EAST  && x < MAZE_SIZE - 1) cells[x + 1][y].walls[WEST]  = present;
        if (dir == WEST  && x > 0)             cells[x - 1][y].walls[EAST]  = present;
    }

    void markVisited(int x, int y) {
        if (x >= 0 && x < MAZE_SIZE && y >= 0 && y < MAZE_SIZE) {
            cells[x][y].visited = true;
        }
    }

    // This matches the Python "Priority Queue" logic
    int getPriorityDirection(int x, int y, int currentHeading) {
        int best_dir = -1;
        bool found_unvisited = false;
        uint8_t min_dist = 255;

        int dx[] = {0, 1, 0, -1};
        int dy[] = {1, 0, -1, 0};

        for (int i = 0; i < 4; i++) {
            if (!cells[x][y].walls[i]) {
                int nx = x + dx[i], ny = y + dy[i];
                if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
                    bool targetVisited = cells[nx][ny].visited;
                    uint8_t targetDist = cells[nx][ny].distance;

                    // Priority 1: Unvisited cells with lower/equal distance
                    // Priority 2: Visited cells with lower distance
                    if (!found_unvisited && !targetVisited) {
                        // First unvisited neighbor found
                        found_unvisited = true;
                        min_dist = targetDist;
                        best_dir = i;
                    } else if (!targetVisited && targetDist < min_dist) {
                        // Better unvisited neighbor
                        min_dist = targetDist;
                        best_dir = i;
                    } else if (!found_unvisited && targetDist < min_dist) {
                        // No unvisited found yet, pick lowest distance visited neighbor
                        min_dist = targetDist;
                        best_dir = i;
                    }
                }
            }
        }
        return best_dir;
    }

    void floodFill(int targetX, int targetY) {
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) cells[x][y].distance = 255;
        }

        // Handle center goal block
        if (targetX == 4 || targetX == 5) {
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
                    int dx[] = {0, 1, 0, -1}, dy[] = {1, 0, -1, 0};
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