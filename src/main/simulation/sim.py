import pygame
import sys
import time

# --- Configuration ---
MAZE_SIZE = 10
CELL_SIZE = 70
WINDOW_SIZE = MAZE_SIZE * CELL_SIZE
WHITE, BLACK = (255, 255, 255), (30, 30, 30)
RED, BLUE, GREEN = (255, 80, 80), (0, 120, 255), (80, 255, 80)
ROBOT_COLOR, PATH_DOT = (255, 165, 0), (255, 215, 0)
WALL_THICKNESS = 6

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
                if y == MAZE_SIZE-1: self.set_wall(x, y, NORTH, True)
                if x == MAZE_SIZE-1: self.set_wall(x, y, EAST, True)
                if y == 0:           self.set_wall(x, y, SOUTH, True)
                if x == 0:           self.set_wall(x, y, WEST, True)
        self.set_wall(0, 0, WEST, True)
        self.set_wall(0, 0, SOUTH, True)
        self.set_wall(0, 0, EAST, True)

    def set_wall(self, x, y, d, state):
        if 0 <= x < MAZE_SIZE and 0 <= y < MAZE_SIZE:
            self.walls[x][y][d] = state
            nx, ny = x + DIR_VECTORS[d][0], y + DIR_VECTORS[d][1]
            if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE:
                self.walls[nx][ny][(d + 2) % 4] = state

    def flood_fill(self):
        self.distance = [[255 for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        for c in [(4,4), (4,5), (5,4), (5,5)]: self.distance[c[0]][c[1]] = 0
        changed = True
        while changed:
            changed = False
            for x in range(MAZE_SIZE):
                for y in range(MAZE_SIZE):
                    if self.distance[x][y] == 255: continue
                    for d, (dx, dy) in DIR_VECTORS.items():
                        nx, ny = x + dx, y + dy
                        if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE and not self.walls[x][y][d]:
                            if self.distance[nx][ny] > self.distance[x][y] + 1:
                                self.distance[nx][ny] = self.distance[x][y] + 1
                                changed = True

def get_closest_direction(mx, my):
    # Determine which cell we are in
    gx, gy = mx // CELL_SIZE, MAZE_SIZE - 1 - (my // CELL_SIZE)
    if not (0 <= gx < MAZE_SIZE and 0 <= gy < MAZE_SIZE): return None, None, None
    
    # Coordinates of the cell's center
    cell_center_x = (gx * CELL_SIZE) + (CELL_SIZE // 2)
    cell_center_y = ((MAZE_SIZE - 1 - gy) * CELL_SIZE) + (CELL_SIZE // 2)
    
    # Distance from center
    dx, dy = mx - cell_center_x, my - cell_center_y
    
    # Determine edge based on largest offset from center
    if abs(dx) > abs(dy):
        direction = EAST if dx > 0 else WEST
    else:
        direction = SOUTH if dy > 0 else NORTH
        
    return gx, gy, direction

def main():
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE + 60))
    font = pygame.font.SysFont('Segoe UI', 18, bold=True)
    dist_font = pygame.font.SysFont('Consolas', 14)
    sim = MazeSimulator()
    rx, ry, active = 0, 0, False

    while True:
        screen.fill(WHITE)
        mx, my = pygame.mouse.get_pos()
        
        # Draw Distances
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                px, py = x*CELL_SIZE, (MAZE_SIZE-1-y)*CELL_SIZE
                if sim.distance[x][y] == 0:
                    pygame.draw.rect(screen, (255, 230, 230), (px, py, CELL_SIZE, CELL_SIZE))
                txt = dist_font.render(str(sim.distance[x][y]), True, (180, 180, 180))
                screen.blit(txt, (px + 5, py + 5))

        # Draw Walls
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                px, py = x*CELL_SIZE, (MAZE_SIZE-1-y)*CELL_SIZE
                if sim.walls[x][y][NORTH]: pygame.draw.line(screen, BLUE, (px, py), (px+CELL_SIZE, py), WALL_THICKNESS)
                if sim.walls[x][y][SOUTH]: pygame.draw.line(screen, BLUE, (px, py+CELL_SIZE), (px+CELL_SIZE, py+CELL_SIZE), WALL_THICKNESS)
                if sim.walls[x][y][EAST]:  pygame.draw.line(screen, BLUE, (px+CELL_SIZE, py), (px+CELL_SIZE, py+CELL_SIZE), WALL_THICKNESS)
                if sim.walls[x][y][WEST]:  pygame.draw.line(screen, BLUE, (px, py), (px, py+CELL_SIZE), WALL_THICKNESS)

        # Draw Preview/Indicator Circle
        if not active and my < WINDOW_SIZE:
            gx, gy, d = get_closest_direction(mx, my)
            if d is not None:
                # Calculate circle position
                cx = (gx * CELL_SIZE) + (CELL_SIZE // 2)
                cy = ((MAZE_SIZE - 1 - gy) * CELL_SIZE) + (CELL_SIZE // 2)
                if d == NORTH: cy -= CELL_SIZE // 2
                elif d == SOUTH: cy += CELL_SIZE // 2
                elif d == EAST: cx += CELL_SIZE // 2
                elif d == WEST: cx -= CELL_SIZE // 2
                pygame.draw.circle(screen, RED, (cx, cy), 10, 2)

        # Draw Ghost Path Dots
        if not active:
            gx, gy = rx, ry
            for _ in range(50):
                best_d, min_v = -1, sim.distance[gx][gy]
                for d, (dx, dy) in DIR_VECTORS.items():
                    if not sim.walls[gx][gy][d] and sim.distance[gx+dx][gy+dy] < min_v:
                        min_v, best_d = sim.distance[gx+dx][gy+dy], d
                if best_d != -1:
                    gx += DIR_VECTORS[best_d][0]; gy += DIR_VECTORS[best_d][1]
                    pygame.draw.circle(screen, PATH_DOT, (gx*CELL_SIZE+35, (MAZE_SIZE-1-gy)*CELL_SIZE+35), 4)
                else: break

        # Draw Robot
        pygame.draw.circle(screen, ROBOT_COLOR, (rx*CELL_SIZE+35, (MAZE_SIZE-1-ry)*CELL_SIZE+35), 22)

        # HUD
        pygame.draw.rect(screen, BLACK, (0, WINDOW_SIZE, WINDOW_SIZE, 60))
        status = "BUILD MODE: Click circles to toggle | ENTER to Start" if not active else "RUNNING... Press R to Reset"
        screen.blit(font.render(status, True, WHITE), (20, WINDOW_SIZE + 15))

        for event in pygame.event.get():
            if event.type == pygame.QUIT: pygame.quit(); sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN and not active:
                if my < WINDOW_SIZE:
                    gx, gy, d = get_closest_direction(mx, my)
                    if d is not None:
                        sim.set_wall(gx, gy, d, not sim.walls[gx][gy][d])
                        sim.flood_fill()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN: active = True
                if event.key == pygame.K_r: rx, ry, active = 0, 0, False

        if active and sim.distance[rx][ry] > 0:
            time.sleep(0.2)
            best_d, min_v = -1, sim.distance[rx][ry]
            for d, (dx, dy) in DIR_VECTORS.items():
                if not sim.walls[rx][ry][d] and sim.distance[rx+dx][ry+dy] < min_v:
                    min_v, best_d = sim.distance[rx+dx][ry+dy], d
            if best_d != -1: rx += DIR_VECTORS[best_d][0]; ry += DIR_VECTORS[best_d][1]

        pygame.display.flip()

if __name__ == "__main__": main()
