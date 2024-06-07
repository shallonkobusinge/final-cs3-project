#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "generate_maze.h"
#include "stack.h"
#include "state.h"
#include "sdl_wrapper.h"

const size_t GRID_WIDTH = 25;
const size_t GRID_HEIGHT = 12;
const size_t NUM_CELLS = GRID_WIDTH * GRID_HEIGHT;
const int GRID_CELL_SIZE = 40;

const size_t MAZE_WINDOW_WIDTH = (GRID_WIDTH * GRID_CELL_SIZE) + 1;
const size_t MAZE_WINDOW_HEIGHT = (GRID_HEIGHT * GRID_CELL_SIZE) + 1;

const size_t NUM_BUILDINGS = 2;

typedef struct state
{
    maze_state_t *maze_state;
} state_t;

typedef struct maze
{
    size_t width, height, cell_size;
    cell_t cells[GRID_HEIGHT][GRID_WIDTH];
    node_t *stack;
} maze_t;

typedef struct building
{
    size_t x;
    size_t y;
} building_t;

typedef struct maze_state
{
    maze_t maze;
    SDL_Rect hider;
    building_t buildings[];
} maze_state_t;

/**
 * Initialize and draw the grid in which the maze will be displayed.
 */
static void init_grid(maze_state_t *maze_state)
{
    render_color((rgb_color_t){230, 230, 230});

    for (size_t x = 0; x < MAZE_WINDOW_WIDTH; x += GRID_CELL_SIZE)
    {
        render_line(x, 0, x, MAZE_WINDOW_HEIGHT);
    }
    for (size_t y = 0; y < MAZE_WINDOW_HEIGHT; y += GRID_CELL_SIZE)
    {
        render_line(0, y, MAZE_WINDOW_WIDTH, y);
    }

    render_color((rgb_color_t){50, 129, 110});
    render_rect(&maze_state->hider);

    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        SDL_Rect cell = {maze_state->buildings[i].x, maze_state->buildings[i].y, GRID_CELL_SIZE / 2, GRID_CELL_SIZE / 2};
        render_color((rgb_color_t){241, 108, 45});
        render_rect(&cell);
    }
}

/**
 * Initializes the maze by marking all cells as unvisited and setting the border cells as visited.
 * and initializes the adjacency matrix and sets the random seed.
 */
static void init_maze(maze_t *maze)
{
    maze->stack = NULL;

    for (size_t y = 0; y < GRID_HEIGHT; ++y)
    {
        for (size_t x = 0; x < GRID_WIDTH; ++x)
        {
            maze->cells[y][x] = (cell_t){
                .box = {x * GRID_CELL_SIZE, y * GRID_CELL_SIZE, GRID_CELL_SIZE, GRID_CELL_SIZE},
                .west = true,
                .north = true,
                .east = true,
                .south = true,
                .visited = false,
                .start = false,
                .end = false};
        }
    }
}

/**
 * Removes the wall between the current cell and its neighbor.
 * Checks whether the neighbor is in the same row or column and draws a line to remove the wall.
 * @param cell current cell
 * @param neighbor cell neighbor
 */
static void remove_wall(cell_t *current, cell_t *next, size_t direction)
{
    switch (direction)
    {
    case 0:
        current->north = false;
        next->south = false;
        break;
    case 1:
        current->east = false;
        next->west = false;
        break;
    case 2:
        current->south = false;
        next->north = false;
        break;
    case 3:
        current->west = false;
        next->east = false;
        break;
    }
}

static size_t get_neighbors(maze_t *maze, size_t x, size_t y, size_t neighbors[][2])
{
    size_t count = 0;

    if (y > 0 && !maze->cells[y - 1][x].visited)
    {
        neighbors[count][0] = x;
        neighbors[count++][1] = y - 1;
    }
    if (x < GRID_WIDTH - 1 && !maze->cells[y][x + 1].visited)
    {
        neighbors[count][0] = x + 1;
        neighbors[count++][1] = y;
    }
    if (y < GRID_HEIGHT - 1 && !maze->cells[y + 1][x].visited)
    {
        neighbors[count][0] = x;
        neighbors[count++][1] = y + 1;
    }
    if (x > 0 && !maze->cells[y][x - 1].visited)
    {
        neighbors[count][0] = x - 1;
        neighbors[count++][1] = y;
    }
    return count;
}

void on_key(char key, key_event_type_t type, double held_time, state_t *state)
{
    if (type == KEY_PRESSED)
    {
        switch (key)
        {
        case LEFT_ARROW:
        {
            if (state->maze_state->hider.x - GRID_CELL_SIZE >= 0)
            {
                state->maze_state->hider.x -= GRID_CELL_SIZE;
                render_rect(&state->maze_state->hider);
            }
            break;
        }
        case RIGHT_ARROW:
        {
            if (state->maze_state->hider.x + GRID_CELL_SIZE < MAZE_WINDOW_WIDTH)
            {
                state->maze_state->hider.x += GRID_CELL_SIZE;
                render_rect(&state->maze_state->hider);
            }
            break;
        }
        case UP_ARROW:
        {
            if (state->maze_state->hider.y - GRID_CELL_SIZE >= 0)
            {
                state->maze_state->hider.y -= GRID_CELL_SIZE;
                render_rect(&state->maze_state->hider);
            }

            break;
        }
        case DOWN_ARROW:
        {
            if (state->maze_state->hider.y + GRID_CELL_SIZE < MAZE_WINDOW_HEIGHT)
            {
                state->maze_state->hider.y += GRID_CELL_SIZE;
                render_rect(&state->maze_state->hider);
            }

            break;
        }
        }
    }
    else if (type == KEY_RELEASED)
    {
        if (key == LEFT_ARROW || key == RIGHT_ARROW)
        {
            printf("NOTHING \n");
        }
    }
}

static void draw_maze(maze_t *maze)
{
    render_color((rgb_color_t){255, 0, 0});

    for (size_t y = 0; y < GRID_HEIGHT; ++y)
    {
        for (size_t x = 0; x < GRID_WIDTH; ++x)
        {
            cell_t *cell = &maze->cells[y][x];
            if (cell->north)
                render_line(cell->box.x, cell->box.y, cell->box.x + cell->box.w, cell->box.y);
            if (cell->east)
                render_line(cell->box.x + cell->box.w, cell->box.y, cell->box.x + cell->box.w, cell->box.y + cell->box.h);
            if (cell->south)
                render_line(cell->box.x, cell->box.y + cell->box.h, cell->box.x + cell->box.w, cell->box.y + cell->box.h);
            if (cell->west)
                render_line(cell->box.x, cell->box.y, cell->box.x, cell->box.y + cell->box.h);
        }
    }
}

static void build_maze(maze_t *maze)
{
    size_t x = 0, y = 0;
    maze->cells[y][x].visited = true;
    push(&maze->stack, x, y);

    while (!is_empty(maze->stack))
    {
        int neighbors[4][2];
        int count = get_neighbors(maze, x, y, neighbors);

        if (count > 0)
        {
            size_t r = rand() % count;
            size_t nx = neighbors[r][0];
            size_t ny = neighbors[r][1];
            size_t direction = (nx == x) ? ((ny > y) ? 2 : 0) : ((nx > x) ? 1 : 3);

            remove_wall(&maze->cells[y][x], &maze->cells[ny][nx], direction);
            maze->cells[ny][nx].visited = true;
            push(&maze->stack, x, y);
            x = nx;
            y = ny;
        }
        else
        {
            pop(&maze->stack, &x, &y);
        }
    }
}

bool generate_maze(maze_state_t *maze_state)
{
    sdl_on_key((key_handler_t)on_key);

    init_grid(maze_state);
    // draw_maze(&maze_state->maze);
}

void buildings_init(maze_state_t *maze_state)
{
    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        size_t rand_x = (rand() % GRID_WIDTH) + 1;
        size_t rand_y = (rand() % GRID_HEIGHT) + 1;

        maze_state->buildings[i] = (building_t){
            .x = ((GRID_WIDTH - rand_x) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
            .y = ((GRID_HEIGHT - rand_y) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
        };
    }
}

maze_state_t *maze_init()
{
    srand(time(NULL));

    maze_state_t *maze_state = malloc(sizeof(maze_state_t) + (sizeof(cell_t) * NUM_BUILDINGS));
    maze_state->hider = (SDL_Rect){(GRID_CELL_SIZE / 4), (GRID_CELL_SIZE / 4), (GRID_CELL_SIZE / 2), (GRID_CELL_SIZE / 2)};

    buildings_init(maze_state);
    // init_maze(&maze_state->maze);
    // build_maze(&maze_state->maze);

    return maze_state;
}
