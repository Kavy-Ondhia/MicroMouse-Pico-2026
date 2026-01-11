import pygame
import sys
import time

# --- Configuration (Mirroring your Configuration.h) ---
MAZE_SIZE = 10
CELL_SIZE = 60
WINDOW_SIZE = MAZE_SIZE * CELL_SIZE
WHITE = (240, 240, 240)
BLACK = (20, 20, 20)
RED = (255, 50, 50)     # Target
GREEN = (50, 255, 50)   # Start
BLUE = (0, 102, 204)    # Walls
TEXT_COLOR = (80, 80, 80)
ROBOT_COLOR = (255, 165, 0) 
ROBOT_ARROW_COLOR = (0, 0, 0)

ROBOT_MOVE_DELAY = 0.25 # Speed of robot

NORTH, EAST, SOUTH, WEST = 0, 1, 2, 3
DIR_VECTORS = {NORTH: (0, 1), EAST: (1, 0), SOUTH: (0, -1), WEST: (-1, 0)}

class MazeSimulator:
    def __init__(self):
        self.walls = [[[False for _ in range(4)] for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        self.distance = [[255 for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        self.init_maze()
        self.flood_fill()

    def init_maze(self):
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                if y == MAZE_SIZE - 1: self.walls[x][y][NORTH] = True
                if x == MAZE_SIZE - 1: self.walls[x][y][EAST] = True
                if y == 0:             self.walls[x][y][SOUTH] = True
                if x == 0:             self.walls[x][y][WEST] = True
        # Competition Start Rule
        self.walls[0][0][WEST] = True
        self.walls[0][0][SOUTH] = True
        self.walls[0][0][EAST] = True 

    def flood_fill(self):
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE): self.distance[x][y] = 255
        # Set 10x10 Center Goals
        self.distance[4][4] = 0; self.distance[4][5] = 0
        self.distance[5][4] = 0; self.distance[5][5] = 0

        changed = True
        while changed:
            changed = False
            for x in range(MAZE_SIZE):
                for y in range(MAZE_SIZE):
                    if self.distance[x][y] == 255: continue
                    for d, (dx, dy) in DIR_VECTORS.items():
                        nx, ny = x + dx, y + dy
                        if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE:
                            if not self.walls[x][y][d]:
                                if self.distance[nx][ny] > self.distance[x][y] + 1:
                                    self.distance[nx][ny] = self.distance[x][y] + 1
                                    changed = True

class Robot:
    def __init__(self, start_x, start_y, start_dir):
        self.x = start_x
        self.y = start_y
        self.direction = start_dir
        self.last_move_time = time.time()
        self.has_reached_goal = False
        self.active = False # New state: Robot waits for user signal

    def move(self, maze_sim: MazeSimulator):
        if not self.active or self.has_reached_goal: return
        
        if time.time() - self.last_move_time > ROBOT_MOVE_DELAY:
            best_move = None
            min_dist = maze_sim.distance[self.x][self.y]

            # Look for neighbor with smallest distance
            for d, (dx, dy) in DIR_VECTORS.items():
                nx, ny = self.x + dx, self.y + dy
                if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE:
                    if not maze_sim.walls[self.x][self.y][d]:
                        if maze_sim.distance[nx][ny] < min_dist:
                            min_dist = maze_sim.distance[nx][ny]
                            best_move = (nx, ny, d)

            if best_move:
                self.x, self.y, self.direction = best_move
                if maze_sim.distance[self.x][self.y] == 0:
                    self.has_reached_goal = True
            self.last_move_time = time.time()

def draw_robot(screen, robot):
    py_x = robot.x * CELL_SIZE + CELL_SIZE // 2
    py_y = (MAZE_SIZE - 1 - robot.y) * CELL_SIZE + CELL_SIZE // 2
    pygame.draw.circle(screen, ROBOT_COLOR, (py_x, py_y), CELL_SIZE // 3)
    # Draw simple direction indicator
    tip = (0,0)
    if robot.direction == NORTH: tip = (py_x, py_y - 15)
    elif robot.direction == SOUTH: tip = (py_x, py_y + 15)
    elif robot.direction == EAST: tip = (py_x + 15, py_y)
    elif robot.direction == WEST: tip = (py_x - 15, py_y)
    pygame.draw.line(screen, ROBOT_ARROW_COLOR, (py_x, py_y), tip, 3)

def main():
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE + 40)) # Extra space for text
    pygame.display.set_caption("Micromouse: Build then Run")
    font = pygame.font.SysFont('Consolas', 18, bold=True)
    sim = MazeSimulator()
    robot = Robot(0, 0, NORTH)

    while True:
        screen.fill(WHITE)
        # Draw HUD info
        status = "BUILD MODE: Click edges. Press ENTER to Start."
        if robot.active: status = "RUNNING... Press R to reset."
        if robot.has_reached_goal: status = "GOAL REACHED! Press R to rebuild."
        text_surf = font.render(status, True, BLACK)
        screen.blit(text_surf, (10, WINDOW_SIZE + 10))

        # --- Draw Maze ---
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                py_y = (MAZE_SIZE - 1 - y) * CELL_SIZE
                py_x = x * CELL_SIZE
                dist_val = sim.distance[x][y]
                color = RED if dist_val == 0 else TEXT_COLOR
                screen.blit(font.render(str(dist_val), True, color), (py_x + 20, py_y + 20))
                # Walls
                if sim.walls[x][y][NORTH]: pygame.draw.line(screen, BLUE, (py_x, py_y), (py_x + CELL_SIZE, py_y), 4)
                if sim.walls[x][y][SOUTH]: pygame.draw.line(screen, BLUE, (py_x, py_y + CELL_SIZE), (py_x + CELL_SIZE, py_y + CELL_SIZE), 4)
                if sim.walls[x][y][EAST]:  pygame.draw.line(screen, BLUE, (py_x + CELL_SIZE, py_y), (py_x + CELL_SIZE, py_y + CELL_SIZE), 4)
                if sim.walls[x][y][WEST]:  pygame.draw.line(screen, BLUE, (py_x, py_y), (py_x, py_y + CELL_SIZE), 4)

        draw_robot(screen, robot)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit(); sys.exit()
            
            if event.type == pygame.MOUSEBUTTONDOWN and not robot.active:
                mx, my = pygame.mouse.get_pos()
                if my < WINDOW_SIZE: # Don't click the HUD
                    gx, gy = mx // CELL_SIZE, MAZE_SIZE - 1 - (my // CELL_SIZE)
                    lx, ly = mx % CELL_SIZE, my % CELL_SIZE
                    dists = [ly, CELL_SIZE-ly, lx, CELL_SIZE-lx] # N, S, W, E
                    min_d = dists.index(min(dists))
                    if min_d == 0: 
                        sim.walls[gx][gy][NORTH] = not sim.walls[gx][gy][NORTH]
                        if gy < MAZE_SIZE-1: sim.walls[gx][gy+1][SOUTH] = sim.walls[gx][gy][NORTH]
                    elif min_d == 1:
                        sim.walls[gx][gy][SOUTH] = not sim.walls[gx][gy][SOUTH]
                        if gy > 0: sim.walls[gx][gy-1][NORTH] = sim.walls[gx][gy][SOUTH]
                    elif min_d == 2:
                        sim.walls[gx][gy][WEST] = not sim.walls[gx][gy][WEST]
                        if gx > 0: sim.walls[gx-1][gy][EAST] = sim.walls[gx][gy][WEST]
                    elif min_d == 3:
                        sim.walls[gx][gy][EAST] = not sim.walls[gx][gy][EAST]
                        if gx < MAZE_SIZE-1: sim.walls[gx+1][gy][WEST] = sim.walls[gx][gy][EAST]
                    sim.flood_fill()

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN: robot.active = True
                if event.key == pygame.K_r: 
                    robot = Robot(0, 0, NORTH)
                if event.key == pygame.K_SPACE: robot.active = not robot.active

        robot.move(sim)
        pygame.display.flip()

if __name__ == "__main__":
    main()
