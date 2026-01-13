#ifndef MAZE_H
#define MAZE_H

#include <Arduino.h>
#include "Configuration.h"

// Define directions as bit indices
#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

struct Cell {
    uint8_t distance;
    // We use a single byte (8 bits). 
    // Bits 0-3 will be walls (N,E,S,W). Bit 4 will be 'visited'.
    uint8_t data; 
};

class MazeManager {
public:
    Cell cells[MAZE_SIZE][MAZE_SIZE];

    MazeManager() { initMaze(); }

    void initMaze() {
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {
                cells[x][y].distance = 255;
                cells[x][y].data = 0; // Clear all walls and visited status
                
                // Set perimeter walls using our new helper
                if (y == MAZE_SIZE - 1) setWall(x, y, NORTH, true);
                if (x == MAZE_SIZE - 1) setWall(x, y, EAST, true);
                if (y == 0)             setWall(x, y, SOUTH, true);
                if (x == 0)             setWall(x, y, WEST, true);
            }
        }
    }

    // --- NEW HELPER FUNCTIONS FOR BITS ---
    
    // Check if a wall exists in a specific direction
    bool hasWall(int x, int y, int dir) {
        // (1 << dir) creates a mask. e.g. if dir is 2 (SOUTH), mask is 00000100
        // We Use bitwise AND (&) to check if that bit is 1
        return (cells[x][y].data & (1 << dir));
    }

    bool isVisited(int x, int y) {
        // We use the 5th bit (index 4) for visited
        return (cells[x][y].data & (1 << 4));
    }

    void markVisited(int x, int y) {
        if (x >= 0 && x < MAZE_SIZE && y >= 0 && y < MAZE_SIZE) {
            // Bitwise OR (|) turns a bit ON
            cells[x][y].data |= (1 << 4);
        }
    }

    void setWall(int x, int y, int dir, bool present) {
        if (x < 0 || x >= MAZE_SIZE || y < 0 || y >= MAZE_SIZE) return;
        
        if (present) {
            cells[x][y].data |= (1 << dir); // Turn bit ON
        } else {
            cells[x][y].data &= ~(1 << dir); // Turn bit OFF
        }

        // Update neighbors
        if (dir == NORTH && y < MAZE_SIZE - 1) {
            if (present) cells[x][y + 1].data |= (1 << SOUTH);
            else         cells[x][y + 1].data &= ~(1 << SOUTH);
        }
        if (dir == SOUTH && y > 0) {
            if (present) cells[x][y - 1].data |= (1 << NORTH);
            else         cells[x][y - 1].data &= ~(1 << NORTH);
        }
        if (dir == EAST && x < MAZE_SIZE - 1) {
            if (present) cells[x + 1][y].data |= (1 << WEST);
            else         cells[x + 1][y].data &= ~(1 << WEST);
        }
        if (dir == WEST && x > 0) {
            if (present) cells[x - 1][y].data |= (1 << EAST);
            else         cells[x - 1][y].data &= ~(1 << EAST);
        }
    }

    // --- LOGIC UPDATES TO MATCH NEW DATA STRUCTURE ---

    int getPriorityDirection(int x, int y, int currentHeading) {
        int best_dir = -1;
        bool found_unvisited = false;
        uint8_t min_dist = 255;
        int dx[] = {0, 1, 0, -1};
        int dy[] = {1, 0, -1, 0};

        for (int i = 0; i < 4; i++) {
            // UPDATED: Use hasWall helper instead of array access
            if (!hasWall(x, y, i)) { 
                int nx = x + dx[i], ny = y + dy[i];
                if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
                    // UPDATED: Use isVisited helper
                    bool targetVisited = isVisited(nx, ny); 
                    uint8_t targetDist = cells[nx][ny].distance;

                    if (!found_unvisited && !targetVisited) {
                        found_unvisited = true; min_dist = targetDist; best_dir = i;
                    } else if (!targetVisited && targetDist < min_dist) {
                        min_dist = targetDist; best_dir = i;
                    } else if (!found_unvisited && targetDist < min_dist) {
                        min_dist = targetDist; best_dir = i;
                    }
                }
            }
        }
        return best_dir;
    }
    
    // (Added simplified getWeightedDirection logic placeholder if needed)
    int getWeightedDirection(int x, int y, int currentHeading, bool returnMode) {
        // Implement similarly using !hasWall(x,y,i)
        // For now, returning -1 to ensure it compiles, you paste your logic here
        return -1; 
    }

    void floodFill(int targetX, int targetY) {
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) cells[x][y].distance = 255;
        }

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
                        // UPDATED: Use hasWall helper
                        if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE && !hasWall(x,y,i)) {
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