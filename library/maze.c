#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "maze.h"
#include "stack.h"
#include "state.h"
#include "sdl_wrapper.h"
#include "landing_page.h"
#include "maze.h"
#include "sound_effect.h"
#include "seeker.h"
#include "asset.h"

const size_t GRID_WIDTH = 22;
const size_t GRID_HEIGHT = 11;
const size_t NUM_CELLS = GRID_WIDTH * GRID_HEIGHT;
const size_t GRID_CELL_SIZE = 45;

const size_t MAZE_WINDOW_WIDTH = (GRID_WIDTH * GRID_CELL_SIZE) + 1;
const size_t MAZE_WINDOW_HEIGHT = (GRID_HEIGHT * GRID_CELL_SIZE) + 1;

const size_t NUM_BUILDINGS = 2;
const size_t TOTAL_GAME_TIME = 120; // IN SECONDS
const char *building_paths[] = {
    "assets/images/scenery/caltech-hall.png",
    "assets/images/scenery/beckman-auditorium.png"};

typedef struct state
{
    scene_t *scene;
    size_t page;
    maze_state_t *maze_state;
    landing_page_state_t *landing_page_state;
    end_page_state_t *end_game_state;
    sound_effect_t *sound_effect;
    seeker_t *seeker;
    list_t *body_assets;
} state_t;

typedef struct maze
{
    size_t width, height, cell_size;
    cell_t **cells;
    node_t *stack;
} maze_t;

typedef struct building
{
    size_t x;
    size_t y;
    const char *path;
} building_t;

typedef struct maze_state
{
    maze_t *maze;
    double time_elapsed;
    building_t buildings[];
} maze_state_t;

/**
 * Create memory for the maze.
 * @return empty maze
 */
maze_t *create_maze()
{
    maze_t *maze = malloc(sizeof(maze_t));
    maze->cells = malloc(GRID_HEIGHT * sizeof(cell_t *));
    for (size_t i = 0; i < GRID_HEIGHT; i++)
    {
        maze->cells[i] = malloc(GRID_WIDTH * sizeof(cell_t));
    }
    return maze;
}

/**
 * Initializes and draws the grid, draws buildings and hider.
 * @param state state struct.
 */
static void init_grid(state_t *state)
{
    maze_state_t *maze_state = state->maze_state;

    render_color((rgb_color_t){230, 230, 230});

    for (size_t x = 0; x < MAZE_WINDOW_WIDTH; x += GRID_CELL_SIZE)
    {
        render_line(x, 0, x, MAZE_WINDOW_HEIGHT);
    }
    for (size_t y = 0; y < MAZE_WINDOW_HEIGHT; y += GRID_CELL_SIZE)
    {
        render_line(0, y, MAZE_WINDOW_WIDTH, y);
    }

    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        vector_t center = (vector_t){.x = maze_state->buildings[i].x, .y = maze_state->buildings[i].y};
        add_to_scene(state, center, (rgb_color_t){241, 108, 45}, building_paths[i]);
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
 * Checks the four possible neighbors (north, east, south, west) of the cell located at
 * coordinates (x, y) in the maze. It adds the coordinates of the unvisited neighbors to the
 * neighbors array and returns the count of unvisited neighbors.
 *
 * @param maze A pointer to the maze structure, which contains the grid of cells.
 * @param x The x-coordinate of the current cell.
 * @param y The y-coordinate of the current cell.
 * @param neighbors A 2D array to store the coordinates of the unvisited neighboring cells. Each
 *                  element is an array of size 2, where the first element is the x-coordinate and
 *                  the second element is the y-coordinate of an unvisited neighbor.
 *
 * @return The number of unvisited neighboring cells.
 */
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

/**
 * Generate maze using DFS algorithm
 * @param maze maze
 */
static void generate_maze(maze_t *maze)
{
    size_t x = 0, y = 0;
    maze->cells[y][x].visited = true;
    push(&maze->stack, x, y);

    while (!is_empty(maze->stack))
    {
        size_t neighbors[4][2];
        size_t count = get_neighbors(maze, x, y, neighbors);

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

/**
 * Initialize and randomly generate a buildings
 * @param maze_state state of the maze
 */
static void buildings_init(maze_state_t *maze_state)
{
    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        size_t rand_x = (rand() % GRID_WIDTH) + 1;
        size_t rand_y = (rand() % GRID_HEIGHT) + 1;

        maze_state->buildings[i] = (building_t){
            .x = ((GRID_WIDTH - rand_x) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 2,
            .y = ((GRID_HEIGHT - rand_y) * GRID_CELL_SIZE) - GRID_CELL_SIZE / 20,
        };
        maze_state->buildings[i].path = building_paths[i];
    }
}

maze_state_t *maze_init()
{
    srand(time(NULL));

    maze_state_t *maze_state = malloc(sizeof(maze_state_t) + (sizeof(cell_t) * NUM_BUILDINGS));
    maze_state->maze = create_maze();
    maze_state->time_elapsed = 0;

    buildings_init(maze_state);

    init_maze(maze_state->maze);
    generate_maze(maze_state->maze);

    return maze_state;
}

/**
 *  Draw maze
 * @param maze maze to draw
 */
static void draw_maze(maze_t *maze)
{
    render_color((rgb_color_t){0, 0, 0});

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

vector_t traverse_maze(state_t *state, vector_t new_vec, size_t movement_direction)
{

    vector_t valid_move = VEC_ZERO;
    maze_t *maze = state->maze_state->maze;

    size_t x = (size_t)new_vec.x / GRID_CELL_SIZE;
    size_t y = GRID_HEIGHT - 1 - ((size_t)new_vec.y / GRID_CELL_SIZE);
    cell_t *cell = &maze->cells[y][x];
    vector_t directions[] = {
        {.x = 0.0, .y = valid_move.y + GRID_CELL_SIZE}, // north
        {.x = valid_move.x + GRID_CELL_SIZE, .y = 0},   // east
        {.x = 0.0, .y = valid_move.y - GRID_CELL_SIZE}, // south
        {.x = valid_move.x - GRID_CELL_SIZE, .y = 0},   // west
    };
    bool walls[] = {
        cell->north,
        cell->east,
        cell->south,
        cell->west,
    };

    if (movement_direction == -1)
    {
        vector_t possible_move[4];
        size_t move_counts = 0;

        for (size_t i = 0; i < 4; i++)
        {
            if (!walls[i])
            {
                possible_move[move_counts++] = directions[i];
            }
        }
        valid_move = possible_move[rand() % move_counts];
    }

    else if (movement_direction >= 0 && movement_direction < 4 && !walls[movement_direction])
    {
        valid_move = directions[movement_direction];
    }
    else
    {
        valid_move = VEC_ZERO;
    }
    return valid_move;
}

void on_key(char key, key_event_type_t type, double held_time, state_t *state)
{
    body_t *beaver = scene_get_body(state->scene, 0);
    vector_t centroid = body_get_centroid(beaver);
    vector_t translation = VEC_ZERO;

    if (type == KEY_PRESSED)
    {
        switch (key)
        {
        case LEFT_ARROW:
        {
            translation = traverse_maze(state, centroid, 3);
            break;
        }
        case RIGHT_ARROW:
        {
            translation = traverse_maze(state, centroid, 1);
            break;
        }
        case UP_ARROW:
        {
            translation = traverse_maze(state, centroid, 0);
            break;
        }
        case DOWN_ARROW:
        {
            translation = traverse_maze(state, centroid, 2);
            break;
        }
        }
    }
    move_body(beaver, translation);
}

/**
 * Display the time elapsed in a human-readable format.
 *
 * This function takes the remaining time in seconds and calculates the
 * time elapsed from a predefined total game time. It then formats and
 * prints the remaining time in minutes and seconds if at least one minute
 * has passed, or just seconds if less than a minute has passed.
 *
 * @param remaining_seconds The remaining time in seconds.
 */
static void display_time_elapsed(int32_t remaining_seconds)
{
    int32_t time_elapsed = TOTAL_GAME_TIME - remaining_seconds;

    int32_t minutes = time_elapsed / 60;
    int32_t seconds = time_elapsed % 60;

    if (minutes < 2)
    {
        if (minutes == 1)
        {
            printf("TIME REMAINING: %d min %d sec\n", minutes, seconds);
        }
        else
        {
            printf("TIME REMAINING: %d sec\n", seconds);
        }
    }
}

void show_maze(state_t *state, double dt)
{
    sdl_on_key((key_handler_t)on_key);
    state->maze_state->time_elapsed += dt;

    display_time_elapsed((int32_t)state->maze_state->time_elapsed);

    init_grid(state);
    draw_maze(state->maze_state->maze);

    // seekers_random_movement(state);
    render_seeker(state, dt);
    render_bodies(state->body_assets);
    seeker_collision(state);
}
