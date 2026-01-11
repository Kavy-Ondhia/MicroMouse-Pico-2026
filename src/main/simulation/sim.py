import pygame
import sys
import time

# --- Configuration ---
MAZE_SIZE = 10
CELL_SIZE = 70
WINDOW_SIZE = MAZE_SIZE * CELL_SIZE
WHITE, BLACK = (255, 255, 255), (30, 30, 30)
RED, BLUE, GREEN = (255, 80, 80), (0, 120, 255), (80, 255, 80)
VISITED_COLOR = (230, 255, 230)
ROBOT_COLOR, PATH_DOT = (255, 165, 0), (255, 215, 0)
WALL_THICKNESS = 6

NORTH, EAST, SOUTH, WEST = 0, 1, 2, 3
DIR_VECTORS = {NORTH: (0, 1), EAST: (1, 0), SOUTH: (0, -1), WEST: (-1, 0)}

class MazeSimulator:
    def __init__(self):
        self.walls = [[[False for _ in range(4)] for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        self.distance = [[255 for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        self.visited = [[False for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        self.target = "CENTER" 
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
        if self.target == "CENTER":
            for c in [(4,4), (4,5), (5,4), (5,5)]: self.distance[c[0]][c[1]] = 0
        else:
            self.distance[0][0] = 0

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
    gx, gy = mx // CELL_SIZE, MAZE_SIZE - 1 - (my // CELL_SIZE)
    if not (0 <= gx < MAZE_SIZE and 0 <= gy < MAZE_SIZE): return None, None, None
    cx, cy = gx * CELL_SIZE + 35, ((MAZE_SIZE - 1 - gy) * CELL_SIZE) + 35
    dx, dy = mx - cx, my - cy
    if abs(dx) > abs(dy): return gx, gy, (EAST if dx > 0 else WEST)
    return gx, gy, (SOUTH if dy > 0 else NORTH)

def main():
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE + 60))
    pygame.display.set_caption("MicroMouse Sim - With Ghost Path")
    font = pygame.font.SysFont('Arial', 18, bold=True)
    dist_font = pygame.font.SysFont('Consolas', 12)
    
    sim = MazeSimulator()
    rx, ry, active = 0, 0, False

    while True:
        screen.fill(WHITE)
        mx, my = pygame.mouse.get_pos()
        
        # 1. Backgrounds & Distances
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                px, py = x * CELL_SIZE, (MAZE_SIZE - 1 - y) * CELL_SIZE
                if sim.visited[x][y]:
                    pygame.draw.rect(screen, VISITED_COLOR, (px, py, CELL_SIZE, CELL_SIZE))
                if x in [4,5] and y in [4,5]:
                    pygame.draw.rect(screen, (255, 220, 220), (px, py, CELL_SIZE, CELL_SIZE))
                txt = dist_font.render(str(sim.distance[x][y] if sim.distance[x][y] < 255 else "X"), True, (150, 150, 150))
                screen.blit(txt, (px + 5, py + 5))

        # 2. Walls
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                px, py = x * CELL_SIZE, (MAZE_SIZE - 1 - y) * CELL_SIZE
                if sim.walls[x][y][NORTH]: pygame.draw.line(screen, BLUE, (px, py), (px+70, py), 6)
                if sim.walls[x][y][SOUTH]: pygame.draw.line(screen, BLUE, (px, py+70), (px+70, py+70), 6)
                if sim.walls[x][y][EAST]:  pygame.draw.line(screen, BLUE, (px+70, py), (px+70, py+70), 6)
                if sim.walls[x][y][WEST]:  pygame.draw.line(screen, BLUE, (px, py), (px, py+70), 6)

        # 3. Yellow Ghost Path Dots (Prediction)
        if not active:
            gx, gy = rx, ry
            temp_visited = [row[:] for row in sim.visited] # Copy visited status
            for _ in range(40):
                if sim.distance[gx][gy] == 0: break
                best_s, best_d = 1000, -1
                for d, (dx, dy) in DIR_VECTORS.items():
                    if not sim.walls[gx][gy][d]:
                        nx, ny = gx+dx, gy+dy
                        score = sim.distance[nx][ny]
                        if sim.target == "START" and not temp_visited[nx][ny]: score -= 3.0 # Stronger bias for prediction
                        if score < best_s: best_s, best_d = score, d
                if best_d != -1:
                    gx += DIR_VECTORS[best_d][0]; gy += DIR_VECTORS[best_d][1]
                    temp_visited[gx][gy] = True
                    pygame.draw.circle(screen, PATH_DOT, (gx*70+35, (MAZE_SIZE-1-gy)*70+35), 5)
                else: break

        # 4. Events
        for event in pygame.event.get():
            if event.type == pygame.QUIT: pygame.quit(); sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN and not active and my < WINDOW_SIZE:
                gx, gy, d = get_closest_direction(mx, my)
                sim.set_wall(gx, gy, d, not sim.walls[gx][gy][d]); sim.flood_fill()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN: active = True
                if event.key == pygame.K_r: rx, ry, active = 0, 0, False; sim.__init__()

        # 5. Robot Logic
        if active:
            sim.visited[rx][ry] = True
            if sim.distance[rx][ry] == 0:
                if sim.target == "CENTER": sim.target = "START"; sim.flood_fill()
                else: active = False
            else:
                time.sleep(0.15)
                best_s, best_d = 1000, -1
                for d, (dx, dy) in DIR_VECTORS.items():
                    if not sim.walls[rx][ry][d]:
                        nx, ny = rx + dx, ry + dy
                        score = sim.distance[nx][ny]
                        if sim.target == "START" and not sim.visited[nx][ny]: score -= 3.0
                        if score < best_s: best_s, best_d = score, d
                if best_d != -1: rx += DIR_VECTORS[best_d][0]; ry += DIR_VECTORS[best_d][1]

        # 6. UI
        pygame.draw.circle(screen, ROBOT_COLOR, (rx*70+35, (MAZE_SIZE-1-ry)*70+35), 22)
        pygame.draw.rect(screen, BLACK, (0, WINDOW_SIZE, WINDOW_SIZE, 60))
        screen.blit(font.render(f"TARGET: {sim.target} | ENTER: Start | R: Reset", True, WHITE), (20, WINDOW_SIZE + 15))
        pygame.display.flip()

if __name__ == "__main__": main()
