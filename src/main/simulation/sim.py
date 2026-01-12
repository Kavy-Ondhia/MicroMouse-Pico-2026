import pygame
import sys
import time

# --- Configuration ---
MAZE_SIZE = 10
CELL_SIZE = 60 
WINDOW_SIZE = MAZE_SIZE * CELL_SIZE
OFFSET = 40 
WHITE, BLACK = (255, 255, 255), (30, 30, 30)
BLUE, YELLOW, DISCOVERED_GREEN = (0, 120, 255), (255, 215, 0), (200, 255, 200)
ROBOT_COLOR = (255, 165, 0)
WALL_HIDDEN = (240, 240, 240) 

NORTH, EAST, SOUTH, WEST = 0, 1, 2, 3
DIR_VECTORS = {NORTH: (0, 1), EAST: (1, 0), SOUTH: (0, -1), WEST: (-1, 0)}

class MazeSimulator:
    def __init__(self, is_truth=False):
        self.walls = [[[False for _ in range(4)] for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        self.distance = [[255 for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        self.visited = [[False for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        if is_truth: self.load_actual_maze()

    def set_wall(self, x, y, d, state):
        if 0 <= x < MAZE_SIZE and 0 <= y < MAZE_SIZE:
            self.walls[x][y][d] = state
            nx, ny = x + DIR_VECTORS[d][0], y + DIR_VECTORS[d][1]
            if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE:
                self.walls[nx][ny][(d + 2) % 4] = state

    def load_actual_maze(self):
        # Perimeter Guard
        for i in range(MAZE_SIZE):
            self.set_wall(i, 0, SOUTH, True)
            self.set_wall(i, MAZE_SIZE-1, NORTH, True)
            self.set_wall(0, i, WEST, True)
            self.set_wall(MAZE_SIZE-1, i, EAST, True)

        # Custom Maze Layout
        self.set_wall(0, 0, EAST, True)
        self.set_wall(0, 0, SOUTH, True)
        self.set_wall(0, 0, WEST, True)
        self.set_wall(0, 1, WEST, True)
        self.set_wall(0, 2, EAST, True)
        self.set_wall(0, 2, WEST, True)
        self.set_wall(0, 3, EAST, True)
        self.set_wall(0, 3, WEST, True)
        self.set_wall(0, 4, EAST, True)
        self.set_wall(0, 4, WEST, True)
        self.set_wall(0, 5, EAST, True)
        self.set_wall(0, 5, WEST, True)
        self.set_wall(0, 6, EAST, True)
        self.set_wall(0, 6, WEST, True)
        self.set_wall(0, 7, EAST, True)
        self.set_wall(0, 7, WEST, True)
        self.set_wall(0, 8, EAST, True)
        self.set_wall(0, 8, WEST, True)
        self.set_wall(0, 9, NORTH, True)
        self.set_wall(0, 9, WEST, True)
        self.set_wall(1, 0, SOUTH, True)
        self.set_wall(1, 1, EAST, True)
        self.set_wall(1, 2, NORTH, True)
        self.set_wall(1, 5, EAST, True)
        self.set_wall(1, 6, NORTH, True)
        self.set_wall(1, 8, NORTH, True)
        self.set_wall(1, 9, NORTH, True)
        self.set_wall(2, 0, EAST, True)
        self.set_wall(2, 0, SOUTH, True)
        self.set_wall(2, 2, EAST, True)
        self.set_wall(2, 3, NORTH, True)
        self.set_wall(2, 5, EAST, True)
        self.set_wall(2, 6, NORTH, True)
        self.set_wall(2, 6, EAST, True)
        self.set_wall(2, 8, NORTH, True)
        self.set_wall(2, 8, EAST, True)
        self.set_wall(2, 9, NORTH, True)
        self.set_wall(3, 0, SOUTH, True)
        self.set_wall(3, 1, EAST, True)
        self.set_wall(3, 3, NORTH, True)
        self.set_wall(3, 4, EAST, True)
        self.set_wall(3, 5, EAST, True)
        self.set_wall(3, 7, EAST, True)
        self.set_wall(3, 8, NORTH, True)
        self.set_wall(3, 9, NORTH, True)
        self.set_wall(4, 0, NORTH, True)
        self.set_wall(4, 0, SOUTH, True)
        self.set_wall(4, 1, NORTH, True)
        self.set_wall(4, 2, NORTH, True)
        self.set_wall(4, 3, NORTH, True)
        self.set_wall(4, 5, NORTH, True)
        self.set_wall(4, 6, EAST, True)
        self.set_wall(4, 8, NORTH, True)
        self.set_wall(4, 9, NORTH, True)
        self.set_wall(5, 0, NORTH, True)
        self.set_wall(5, 0, SOUTH, True)
        self.set_wall(5, 2, NORTH, True)
        self.set_wall(5, 2, EAST, True)
        self.set_wall(5, 3, NORTH, True)
        self.set_wall(5, 3, EAST, True)
        self.set_wall(5, 4, EAST, True)
        self.set_wall(5, 5, NORTH, True)
        self.set_wall(5, 7, NORTH, True)
        self.set_wall(5, 8, NORTH, True)
        self.set_wall(5, 9, NORTH, True)
        self.set_wall(5, 9, EAST, True)
        self.set_wall(6, 0, NORTH, True)
        self.set_wall(6, 0, EAST, True)
        self.set_wall(6, 0, SOUTH, True)
        self.set_wall(6, 1, EAST, True)
        self.set_wall(6, 2, NORTH, True)
        self.set_wall(6, 5, NORTH, True)
        self.set_wall(6, 6, NORTH, True)
        self.set_wall(6, 7, EAST, True)
        self.set_wall(6, 9, NORTH, True)
        self.set_wall(6, 9, EAST, True)
        self.set_wall(7, 0, SOUTH, True)
        self.set_wall(7, 1, EAST, True)
        self.set_wall(7, 2, NORTH, True)
        self.set_wall(7, 4, NORTH, True)
        self.set_wall(7, 4, EAST, True)
        self.set_wall(7, 5, NORTH, True)
        self.set_wall(7, 8, EAST, True)
        self.set_wall(7, 9, NORTH, True)
        self.set_wall(8, 0, NORTH, True)
        self.set_wall(8, 0, SOUTH, True)
        self.set_wall(8, 2, NORTH, True)
        self.set_wall(8, 3, NORTH, True)
        self.set_wall(8, 4, EAST, True)
        self.set_wall(8, 6, NORTH, True)
        self.set_wall(8, 6, EAST, True)
        self.set_wall(8, 7, NORTH, True)
        self.set_wall(8, 8, EAST, True)
        self.set_wall(8, 9, NORTH, True)
        self.set_wall(8, 9, EAST, True)
        self.set_wall(9, 0, EAST, True)
        self.set_wall(9, 0, SOUTH, True)
        self.set_wall(9, 1, NORTH, True)
        self.set_wall(9, 1, EAST, True)
        self.set_wall(9, 2, EAST, True)
        self.set_wall(9, 3, EAST, True)
        self.set_wall(9, 4, NORTH, True)
        self.set_wall(9, 4, EAST, True)
        self.set_wall(9, 5, NORTH, True)
        self.set_wall(9, 5, EAST, True)
        self.set_wall(9, 6, EAST, True)
        self.set_wall(9, 7, EAST, True)
        self.set_wall(9, 8, EAST, True)
        self.set_wall(9, 9, NORTH, True)
        self.set_wall(9, 9, EAST, True)

    def flood_fill(self, targets):
        self.distance = [[255 for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        for tx, ty in targets: self.distance[tx][ty] = 0
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
    pygame.display.set_caption("Micromouse: Priority Return Journey")
    font = pygame.font.SysFont('Consolas', 14, bold=True)
    
    truth = MazeSimulator(is_truth=True)
    sim = MazeSimulator(is_truth=False)
    rx, ry, active = 0, 0, False
    
    phase = "START_TO_END_1"
    goal_cells = [(4,4), (4,5), (5,5), (5,4)]
    goal_idx = 0
    sim.flood_fill(goal_cells)

    while True:
        screen.fill(WHITE)
        
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                px, py = OFFSET + x * CELL_SIZE, (MAZE_SIZE - 1 - y) * CELL_SIZE
                if x in [4,5] and y in [4,5]:
                    pygame.draw.rect(screen, (255, 200, 200), (px, py, CELL_SIZE, CELL_SIZE))
                elif x == 0 and y == 0:
                    pygame.draw.rect(screen, (200, 200, 255), (px, py, CELL_SIZE, CELL_SIZE))
                elif sim.visited[x][y]:
                    pygame.draw.rect(screen, DISCOVERED_GREEN, (px, py, CELL_SIZE, CELL_SIZE))
                
                val = sim.distance[x][y]
                txt = font.render(str(val if val < 255 else ""), True, (160, 160, 160))
                screen.blit(txt, (px + 5, py + 5))

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

        if active:
            sim.visited[rx][ry] = True
            found = False
            for d in range(4):
                if truth.walls[rx][ry][d] and not sim.walls[rx][ry][d]:
                    sim.set_wall(rx, ry, d, True); found = True
            
            if phase == "START_TO_END_1":
                if found: sim.flood_fill(goal_cells)
                if rx in [4,5] and ry in [4,5]: phase = "GOAL_SCAN"; goal_idx = 0
                else:
                    time.sleep(0.1)
                    best_d, min_v = -1, sim.distance[rx][ry]
                    for d, (dx, dy) in DIR_VECTORS.items():
                        if not sim.walls[rx][ry][d]:
                            nx, ny = rx+dx, ry+dy
                            if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE:
                                if sim.distance[nx][ny] < min_v: min_v, best_d = sim.distance[nx][ny], d
                    if best_d != -1: rx += DIR_VECTORS[best_d][0]; ry += DIR_VECTORS[best_d][1]

            elif phase == "GOAL_SCAN":
                time.sleep(0.2)
                if goal_idx < len(goal_cells):
                    rx, ry = goal_cells[goal_idx]; goal_idx += 1
                else:
                    phase = "END_TO_START_1"; sim.flood_fill([(0,0)])

            elif phase == "END_TO_START_1":
                if found: sim.flood_fill([(0,0)])
                if rx == 0 and ry == 0: 
                    active = False 
                    phase = "START_TO_END_2"
                    sim.flood_fill(goal_cells)
                else:
                    time.sleep(0.1)
                    min_v = sim.distance[rx][ry]
                    moves = [d for d in range(4) if not sim.walls[rx][ry][d] and sim.distance[rx + DIR_VECTORS[d][0]][ry + DIR_VECTORS[d][1]] < min_v]
                    if moves:
                        unvisited = [d for d in moves if not sim.visited[rx + DIR_VECTORS[d][0]][ry + DIR_VECTORS[d][1]]]
                        chosen = unvisited[0] if unvisited else moves[0]
                        rx += DIR_VECTORS[chosen][0]; ry += DIR_VECTORS[chosen][1]

            elif phase == "START_TO_END_2":
                if found: sim.flood_fill(goal_cells)
                if rx in [4,5] and ry in [4,5]: 
                    phase = "END_TO_START_2"
                    sim.flood_fill([(0,0)]) 
                else:
                    time.sleep(0.1)
                    min_v = sim.distance[rx][ry]
                    best_dirs = [d for d in range(4) if not sim.walls[rx][ry][d] and sim.distance[rx + DIR_VECTORS[d][0]][ry + DIR_VECTORS[d][1]] < min_v]
                    if best_dirs:
                        chosen_dir = min(best_dirs, key=lambda d: abs((rx + DIR_VECTORS[d][0]) - (ry + DIR_VECTORS[d][1])))
                        rx += DIR_VECTORS[chosen_dir][0]; ry += DIR_VECTORS[chosen_dir][1]

            elif phase == "END_TO_START_2":
                if rx == 0 and ry == 0: 
                    active = False
                else:
                    time.sleep(0.1)
                    if found: sim.flood_fill([(0,0)]) # Keep map updated if new walls seen
                    
                    # PRIORITY QUEUE LOGIC:
                    # 1. Identify all reachable neighboring cells
                    valid_moves = []
                    for d in range(4):
                        if not sim.walls[rx][ry][d]:
                            nx, ny = rx + DIR_VECTORS[d][0], ry + DIR_VECTORS[d][1]
                            if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE:
                                # Store: (is_unvisited, distance_to_home, direction)
                                # False comes before True in sorting, so we use 'visited' as the first key
                                valid_moves.append((sim.visited[nx][ny], sim.distance[nx][ny], d))
                    
                    if valid_moves:
                        # Sort by: 1. Unvisited first, 2. Lower distance second
                        valid_moves.sort() 
                        best_d = valid_moves[0][2]
                        rx += DIR_VECTORS[best_d][0]; ry += DIR_VECTORS[best_d][1]

        pygame.draw.circle(screen, ROBOT_COLOR, (OFFSET + rx*CELL_SIZE+CELL_SIZE//2, (MAZE_SIZE-1-ry)*CELL_SIZE+CELL_SIZE//2), 15)
        pygame.draw.rect(screen, BLACK, (0, WINDOW_SIZE + OFFSET, WINDOW_SIZE + OFFSET, 60))
        
        status = f"PHASE: {phase}"
        if not active:
            if rx == 0 and ry == 0: status += " | PRESS ENTER TO START RUN"
            else: status += " | FINISHED"
        
        msg = font.render(status, True, WHITE)
        screen.blit(msg, (20, WINDOW_SIZE + OFFSET + 20))

        for event in pygame.event.get():
            if event.type == pygame.QUIT: pygame.quit(); sys.exit()
            if event.type == pygame.KEYDOWN and event.key == pygame.K_RETURN: active = True

        pygame.display.flip()

if __name__ == "__main__": main()
