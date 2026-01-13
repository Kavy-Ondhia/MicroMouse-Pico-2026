import pygame
import sys
import time

# --- Configuration ---
MAZE_SIZE = 10
CELL_SIZE = 60 # Reduced slightly to fit grid labels on screen
WINDOW_SIZE = MAZE_SIZE * CELL_SIZE
OFFSET = 40 # Offset for coordinate labels
WHITE, BLACK = (255, 255, 255), (30, 30, 30)
BLUE, YELLOW = (0, 120, 255), (255, 215, 0)
ROBOT_COLOR = (255, 165, 0)
WALL_HIDDEN = (240, 240, 240) 

NORTH, EAST, SOUTH, WEST = 0, 1, 2, 3
DIR_VECTORS = {NORTH: (0, 1), EAST: (1, 0), SOUTH: (0, -1), WEST: (-1, 0)}

class MazeSimulator:
    def __init__(self, is_truth=False):
        self.walls = [[[False for _ in range(4)] for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        self.distance = [[255 for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        if is_truth:
            self.load_actual_maze()

    def set_wall(self, x, y, d, state):
        if 0 <= x < MAZE_SIZE and 0 <= y < MAZE_SIZE:
            self.walls[x][y][d] = state
            nx, ny = x + DIR_VECTORS[d][0], y + DIR_VECTORS[d][1]
            if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE:
                self.walls[nx][ny][(d + 2) % 4] = state

    def load_actual_maze(self):
        """EXACT COORDINATES PROVIDED BY USER"""
        # Mapping your specific wall data
        walls_data = [
            (0, 0, 1), (0, 0, 2), (0, 0, 3), (0, 1, 1), (0, 1, 3), (0, 2, 3), (0, 3, 1), (0, 3, 3),
            (0, 4, 1), (0, 4, 3), (0, 5, 1), (0, 5, 3), (0, 6, 1), (0, 6, 3), (0, 7, 1), (0, 7, 3),
            (0, 8, 1), (0, 8, 3), (0, 9, 0), (0, 9, 3), (1, 0, 2), (1, 2, 0), (1, 2, 1), (1, 3, 1),
            (1, 6, 1), (1, 7, 1), (1, 8, 1), (1, 9, 0), (2, 0, 2), (2, 1, 1), (2, 3, 0), (2, 4, 0),
            (2, 6, 1), (2, 7, 0), (2, 7, 1), (2, 9, 0), (3, 0, 2), (3, 2, 1), (3, 3, 1), (3, 4, 0),
            (3, 4, 1), (3, 5, 1), (3, 7, 0), (3, 9, 0), (4, 0, 2), (4, 3, 0), (4, 3, 1), (4, 9, 0),
            (5, 0, 2), (5, 4, 1), (5, 5, 0), (5, 5, 1), (5, 9, 0), (6, 0, 2), (6, 9, 0), (7, 0, 2),
            (7, 9, 0), (8, 0, 2), (8, 9, 0), (9, 0, 1), (9, 0, 2), (9, 1, 1), (9, 2, 1), (9, 3, 1),
            (9, 4, 1), (9, 5, 1), (9, 6, 1), (9, 7, 1), (9, 8, 1), (9, 9, 0), (9, 9, 1)
        ]
        for x, y, d in walls_data:
            self.set_wall(x, y, d, True)

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

def main():
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_SIZE + OFFSET, WINDOW_SIZE + OFFSET + 60))
    pygame.display.set_caption("Micromouse: Discovery with Provided Coordinates")
    font = pygame.font.SysFont('Consolas', 14, bold=True)
    
    truth = MazeSimulator(is_truth=True)
    sim = MazeSimulator(is_truth=False)
    rx, ry, active = 0, 0, False
    sim.flood_fill()

    while True:
        screen.fill(WHITE)
        
        # Draw Coordinate Labels
        for i in range(MAZE_SIZE):
            # X-axis
            lbl = font.render(str(i), True, BLACK)
            screen.blit(lbl, (OFFSET + i*CELL_SIZE + CELL_SIZE//2, WINDOW_SIZE + 5))
            # Y-axis
            lbl = font.render(str(i), True, BLACK)
            screen.blit(lbl, (15, (MAZE_SIZE-1-i)*CELL_SIZE + CELL_SIZE//2))

        # 1. Backgrounds & Distances
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                px, py = OFFSET + x * CELL_SIZE, (MAZE_SIZE - 1 - y) * CELL_SIZE
                if x in [4,5] and y in [4,5]:
                    pygame.draw.rect(screen, (255, 220, 220), (px, py, CELL_SIZE, CELL_SIZE))
                val = sim.distance[x][y]
                txt = font.render(str(val if val < 255 else ""), True, (200, 200, 200))
                screen.blit(txt, (px + 5, py + 5))

        # 2. Draw Walls
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                px, py = OFFSET + x * CELL_SIZE, (MAZE_SIZE - 1 - y) * CELL_SIZE
                for d in range(4):
                    if truth.walls[x][y][d]:
                        color = BLUE if sim.walls[x][y][d] else WALL_HIDDEN
                        w = 4 if sim.walls[x][y][d] else 1
                        if d == NORTH: pygame.draw.line(screen, color, (px, py), (px+CELL_SIZE, py), w)
                        if d == SOUTH: pygame.draw.line(screen, color, (px, py+CELL_SIZE), (px+CELL_SIZE, py+CELL_SIZE), w)
                        if d == EAST:  pygame.draw.line(screen, color, (px+CELL_SIZE, py), (px+CELL_SIZE, py+CELL_SIZE), w)
                        if d == WEST:  pygame.draw.line(screen, color, (px, py), (px, py+CELL_SIZE), w)

        # 3. Ghost Path
        gx, gy = rx, ry
        for _ in range(50):
            if sim.distance[gx][gy] == 0: break
            best_d, min_v = -1, sim.distance[gx][gy]
            for d, (dx, dy) in DIR_VECTORS.items():
                if not sim.walls[gx][gy][d]:
                    nx, ny = gx + dx, gy + dy
                    if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE and sim.distance[nx][ny] < min_v:
                        min_v, best_d = sim.distance[nx][ny], d
            if best_d != -1:
                gx += DIR_VECTORS[best_d][0]; gy += DIR_VECTORS[best_d][1]
                pygame.draw.circle(screen, YELLOW, (OFFSET + gx*CELL_SIZE+CELL_SIZE//2, (MAZE_SIZE-1-gy)*CELL_SIZE+CELL_SIZE//2), 4)
            else: break

        # 4. Movement Logic
        if active:
            found = False
            for d in range(4):
                if truth.walls[rx][ry][d] and not sim.walls[rx][ry][d]:
                    sim.set_wall(rx, ry, d, True); found = True
            if found: sim.flood_fill()
            if sim.distance[rx][ry] == 0: active = False
            else:
                time.sleep(0.2)
                best_d, min_v = -1, sim.distance[rx][ry]
                for d, (dx, dy) in DIR_VECTORS.items():
                    if not sim.walls[rx][ry][d]:
                        nx, ny = rx+dx, ry+dy
                        if sim.distance[nx][ny] < min_v:
                            min_v, best_d = sim.distance[nx][ny], d
                if best_d != -1: rx += DIR_VECTORS[best_d][0]; ry += DIR_VECTORS[best_d][1]

        # 5. Robot & UI
        pygame.draw.circle(screen, ROBOT_COLOR, (OFFSET + rx*CELL_SIZE+CELL_SIZE//2, (MAZE_SIZE-1-ry)*CELL_SIZE+CELL_SIZE//2), 15)
        pygame.draw.rect(screen, BLACK, (0, WINDOW_SIZE + OFFSET, WINDOW_SIZE + OFFSET, 60))
        msg = font.render("ENTER: Start | R: Reset | Yellow: Ghost Path", True, WHITE)
        screen.blit(msg, (20, WINDOW_SIZE + OFFSET + 20))

        for event in pygame.event.get():
            if event.type == pygame.QUIT: pygame.quit(); sys.exit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN: active = True
                if event.key == pygame.K_r: rx, ry, active = 0, 0, False; sim = MazeSimulator(); sim.flood_fill()

        pygame.display.flip()

if __name__ == "__main__": main()
