import pygame
import sys

# --- Configuration ---
MAZE_SIZE = 10
CELL_SIZE = 70
WINDOW_SIZE = MAZE_SIZE * CELL_SIZE
WHITE = (255, 255, 255)
BLACK = (30, 30, 30)
BLUE = (0, 120, 255)      # Active Wall Color
RED_HIGHLIGHT = (255, 100, 100) # Hover Color
GRID_COLOR = (220, 220, 220)

# Directions
NORTH, EAST, SOUTH, WEST = 0, 1, 2, 3
DIR_NAMES = {NORTH: "NORTH", EAST: "EAST", SOUTH: "SOUTH", WEST: "WEST"}
DIR_VECTORS = {NORTH: (0, 1), EAST: (1, 0), SOUTH: (0, -1), WEST: (-1, 0)}

class MazeBuilder:
    def __init__(self):
        # 3D array: walls[x][y][direction]
        self.walls = [[[False for _ in range(4)] for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        self.init_borders()

    def init_borders(self):
        # Set the absolute outer limits (cannot be removed usually, but editable here)
        for i in range(MAZE_SIZE):
            self.set_wall(i, MAZE_SIZE-1, NORTH, True)
            self.set_wall(MAZE_SIZE-1, i, EAST, True)
            self.set_wall(i, 0, SOUTH, True)
            self.set_wall(0, i, WEST, True)

    def set_wall(self, x, y, d, state):
        if 0 <= x < MAZE_SIZE and 0 <= y < MAZE_SIZE:
            self.walls[x][y][d] = state
            # Update the neighbor wall to match
            nx, ny = x + DIR_VECTORS[d][0], y + DIR_VECTORS[d][1]
            if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE:
                self.walls[nx][ny][(d + 2) % 4] = state

    def export_walls(self):
        print("\n" + "="*40)
        print("   COPY THE CODE BELOW THIS LINE")
        print("="*40)
        
        # We only export NORTH and EAST to avoid duplicates (Cell A North = Cell B South)
        # We iterate everything to be safe and explicit for your other code
        print(f"    # Custom Maze Layout Exported")
        
        # To avoid duplicates in the printout, we track what we've printed
        printed_walls = set()

        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                for d in range(4):
                    if self.walls[x][y][d]:
                        # Sort coordinates to ensure (A -> B) is same as (B -> A)
                        nx, ny = x + DIR_VECTORS[d][0], y + DIR_VECTORS[d][1]
                        
                        # Create a unique signature for this wall to avoid double printing
                        # E.g. Wall between (0,0) and (0,1)
                        if (x, y, d) not in printed_walls:
                            print(f"        self.set_wall({x}, {y}, {DIR_NAMES[d]}, True)")
                            
                            # Mark this wall AND its neighbor counterpart as printed
                            printed_walls.add((x, y, d))
                            if 0 <= nx < MAZE_SIZE and 0 <= ny < MAZE_SIZE:
                                printed_walls.add((nx, ny, (d+2)%4))

        print("="*40 + "\n")

def get_closest_edge(mx, my):
    """
    Returns grid_x, grid_y, and direction (N/E/S/W) based on mouse position.
    """
    # 1. Determine Grid Coordinates
    # Note: PyGame draws (0,0) at top-left.
    # We want our logic (0,0) at bottom-left.
    gx = mx // CELL_SIZE
    gy = MAZE_SIZE - 1 - (my // CELL_SIZE) # Flip Y axis logic
    
    if not (0 <= gx < MAZE_SIZE and 0 <= gy < MAZE_SIZE):
        return None, None, None

    # 2. Determine relative position within the cell (0 to CELL_SIZE)
    # Screen Y for the top of this cell
    cell_screen_y = (MAZE_SIZE - 1 - gy) * CELL_SIZE
    cell_screen_x = gx * CELL_SIZE
    
    rel_x = mx - cell_screen_x
    rel_y = my - cell_screen_y
    
    # 3. Find closest edge
    # Distance to edges: Top(0), Bottom(Size), Left(0), Right(Size)
    dist_N = rel_y                # Distance from top edge (screen y increases down)
    dist_S = CELL_SIZE - rel_y    # Distance from bottom edge
    dist_W = rel_x                # Distance from left
    dist_E = CELL_SIZE - rel_x    # Distance from right
    
    m = min(dist_N, dist_S, dist_W, dist_E)
    
    if m == dist_N: return gx, gy, NORTH
    if m == dist_S: return gx, gy, SOUTH
    if m == dist_W: return gx, gy, WEST
    if m == dist_E: return gx, gy, EAST
    
    return None, None, None

def main():
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE + 60))
    pygame.display.set_caption("Maze Builder - Press ENTER to Export Code")
    font = pygame.font.SysFont('Consolas', 16)
    
    builder = MazeBuilder()
    
    # Add Start Area Caps (L-Shape usually closed off) - Optional defaults
    # builder.set_wall(0, 0, EAST, True) 
    
    while True:
        screen.fill(WHITE)
        
        # --- Draw Grid Guide ---
        for x in range(MAZE_SIZE + 1):
            pygame.draw.line(screen, GRID_COLOR, (x * CELL_SIZE, 0), (x * CELL_SIZE, WINDOW_SIZE), 1)
        for y in range(MAZE_SIZE + 1):
            pygame.draw.line(screen, GRID_COLOR, (0, y * CELL_SIZE), (WINDOW_SIZE, y * CELL_SIZE), 1)

        # --- Draw Coordinates Text ---
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                px, py = x * CELL_SIZE, (MAZE_SIZE - 1 - y) * CELL_SIZE
                if x == 0 or y == 0:
                    lbl = font.render(f"{x},{y}", True, (200, 200, 200))
                    screen.blit(lbl, (px + 5, py + 50))

        # --- Draw Existing Walls ---
        for x in range(MAZE_SIZE):
            for y in range(MAZE_SIZE):
                px, py = x * CELL_SIZE, (MAZE_SIZE - 1 - y) * CELL_SIZE
                
                # Draw lines if wall exists
                # NORTH (Top in PyGame is low Y, but our logic says North is Up... wait)
                # Correction: logic y=0 is bottom.
                # In PyGame screen:
                # Cell (x,y) North edge is the top line of the drawn square.
                
                width = 6
                
                if builder.walls[x][y][NORTH]:
                    pygame.draw.line(screen, BLUE, (px, py), (px + CELL_SIZE, py), width)
                if builder.walls[x][y][SOUTH]:
                    pygame.draw.line(screen, BLUE, (px, py + CELL_SIZE), (px + CELL_SIZE, py + CELL_SIZE), width)
                if builder.walls[x][y][EAST]:
                    pygame.draw.line(screen, BLUE, (px + CELL_SIZE, py), (px + CELL_SIZE, py + CELL_SIZE), width)
                if builder.walls[x][y][WEST]:
                    pygame.draw.line(screen, BLUE, (px, py), (px, py + CELL_SIZE), width)

        # --- Hover Interaction ---
        mx, my = pygame.mouse.get_pos()
        if my < WINDOW_SIZE:
            gx, gy, d = get_closest_edge(mx, my)
            if gx is not None:
                # Calculate screen coordinates for the highlight
                px, py = gx * CELL_SIZE, (MAZE_SIZE - 1 - gy) * CELL_SIZE
                
                # Draw a red guide line
                if d == NORTH: pygame.draw.line(screen, RED_HIGHLIGHT, (px, py), (px + CELL_SIZE, py), 4)
                if d == SOUTH: pygame.draw.line(screen, RED_HIGHLIGHT, (px, py + CELL_SIZE), (px + CELL_SIZE, py + CELL_SIZE), 4)
                if d == EAST:  pygame.draw.line(screen, RED_HIGHLIGHT, (px + CELL_SIZE, py), (px + CELL_SIZE, py + CELL_SIZE), 4)
                if d == WEST:  pygame.draw.line(screen, RED_HIGHLIGHT, (px, py), (px, py + CELL_SIZE), 4)

        # --- Instructions ---
        pygame.draw.rect(screen, BLACK, (0, WINDOW_SIZE, WINDOW_SIZE, 60))
        msg = font.render("CLICK walls to toggle. Press ENTER to print coordinates to Console.", True, WHITE)
        screen.blit(msg, (20, WINDOW_SIZE + 20))

        # --- Events ---
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit(); sys.exit()
            
            if event.type == pygame.MOUSEBUTTONDOWN:
                if my < WINDOW_SIZE:
                    gx, gy, d = get_closest_edge(mx, my)
                    if gx is not None:
                        current_state = builder.walls[gx][gy][d]
                        builder.set_wall(gx, gy, d, not current_state)
            
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN:
                    builder.export_walls()

        pygame.display.flip()

if __name__ == "__main__":
    main()
