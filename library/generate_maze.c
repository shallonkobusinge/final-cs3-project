#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cell.h"
#include "generate_maze.h"
#include "stack.h"
#include "state.h"
#include "sdl_wrapper.h"

const size_t GRID_WIDTH = 25;
const size_t GRID_HEIGHT = 12;
const size_t NUM_CELLS = GRID_WIDTH * GRID_HEIGHT;

const int GRID_CELL_SIZE = 40;
const int window_width = (GRID_WIDTH * GRID_CELL_SIZE) + 1;
const int window_height = (GRID_HEIGHT * GRID_CELL_SIZE) + 1;

bool visited[GRID_WIDTH + 2][GRID_HEIGHT + 2];
bool adj_matrix[NUM_CELLS][NUM_CELLS];
cell_t *parent[GRID_WIDTH][GRID_HEIGHT];

SDL_Rect hider_cell = (SDL_Rect){(GRID_CELL_SIZE / 4), (GRID_CELL_SIZE / 4), (GRID_CELL_SIZE / 2), (GRID_CELL_SIZE / 2)};

typedef struct state
{
    scene_t *scene;
    size_t page;
    bool maze_generated;
    int counter;
    cell_t random_cell;
    stack_t *head;
} state_t;

typedef struct maze_state
{
    stack_t *head;
    bool visited[GRID_WIDTH + 2][GRID_HEIGHT + 2];
    bool adj_matrix[NUM_CELLS][NUM_CELLS];
    cell_t buildings[];

} maze_state_t;

const size_t NUM_BUILDINGS = 4;
static int counting = 0;

/**
 * Finds max between two numbers
 * @param a first number
 * @param b second number
 * @return maximum numbers between the provided arguments
 */
static size_t
find_max(size_t a, size_t b)
{
    return (a > b) ? a : b;
}

/**
 * Finds min between two numbers
 * @param a first number
 * @param b second number
 * @return minimum numbers between the provided arguments
 */
static size_t find_min(size_t a, size_t b)
{
    return (a > b) ? b : a;
}

/**
 * Initialize and draw the Maze Grid.
 */
static void init_grid(maze_state_t *state)
{
    render_color((rgb_color_t){230, 230, 230});

    for (int x = 0; x < window_width; x += GRID_CELL_SIZE)
    {
        render_line(x, 0, x, window_height);
    }
    for (int y = 0; y < window_height; y += GRID_CELL_SIZE)
    {
        render_line(0, y, window_width, y);
    }

    render_color((rgb_color_t){50, 129, 110});
    render_rect(&hider_cell);

    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        SDL_Rect cell = {state->buildings[i].x, state->buildings[i].y, GRID_CELL_SIZE / 2, GRID_CELL_SIZE / 2};
        render_color((rgb_color_t){241, 108, 45});
        render_rect(&cell);
    }
}

/**
 * Initializes the maze by marking all cells as unvisited and setting the border cells as visited.
 * and initializes the adjacency matrix and sets the random seed.
 */
static void init_maze(maze_state_t *maze_state)
{
    for (int i = 1; i <= GRID_WIDTH; i++)
    {
        for (int j = 1; j <= GRID_HEIGHT; j++)
        {
            maze_state->visited[i][j] = 0;
        }
    }
    for (int j = 1; j < GRID_HEIGHT + 2; j++)
    {
        maze_state->visited[0][j] = true;
        maze_state->visited[GRID_WIDTH + 1][j] = true;
    }

    for (int i = 1; i <= GRID_WIDTH; i++)
    {
        maze_state->visited[i][0] = true;
        maze_state->visited[i][GRID_HEIGHT + 1] = true;
    }

    for (int i = 0; i < NUM_CELLS; i++)
    {
        for (int j = 0; j < NUM_CELLS; j++)
        {
            maze_state->adj_matrix[i][j] = true;
            printf("Adj at (%zu)", maze_state->adj_matrix[i][j]);
        }
    }
}

void on_key(char key, key_event_type_t type, double held_time, state_t *state)
{
    if (type == KEY_PRESSED)
    {
        switch (key)
        {
        case LEFT_ARROW:
        {
            if (hider_cell.x - GRID_CELL_SIZE >= 0)
            {
                hider_cell.x -= GRID_CELL_SIZE;
                render_rect(&hider_cell);
            }
            break;
        }
        case RIGHT_ARROW:
        {
            if (hider_cell.x + GRID_CELL_SIZE < window_width)
            {
                hider_cell.x += GRID_CELL_SIZE;
                render_rect(&hider_cell);
            }
            break;
        }
        case UP_ARROW:
        {
            if (hider_cell.y - GRID_CELL_SIZE >= 0)
            {
                hider_cell.y -= GRID_CELL_SIZE;
                render_rect(&hider_cell);
            }

            break;
        }
        case DOWN_ARROW:
        {
            if (hider_cell.y + GRID_CELL_SIZE < window_height)
            {
                hider_cell.y += GRID_CELL_SIZE;
                render_rect(&hider_cell);
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

/**
 * Removes the wall between the current cell and its neighbor.
 * Checks whether the neighbor is in the same row or column and draws a line to remove the wall.
 * @param cell current cell
 * @param neighbor cell neighbor
 */
void remove_wall(cell_t *cell, cell_t *neighbor)
{
    if (cell->x == neighbor->x)
    {
        render_color((rgb_color_t){22, 22, 22});
        size_t y = find_max(cell->y, neighbor->y);
        render_line((cell->x - 1) * GRID_CELL_SIZE,
                    (y - 1) * GRID_CELL_SIZE,
                    (cell->x - 1) * GRID_CELL_SIZE + GRID_CELL_SIZE,
                    (y - 1) * GRID_CELL_SIZE);
    }

    else if (cell->y == neighbor->y)
    {
        render_color((rgb_color_t){22, 22, 22});

        int x = find_max(cell->x, neighbor->x);
        render_line((x - 1) * GRID_CELL_SIZE,
                    (cell->y - 1) * GRID_CELL_SIZE,
                    (x - 1) * GRID_CELL_SIZE,
                    (cell->y - 1) * GRID_CELL_SIZE + GRID_CELL_SIZE);
    }
    // printf("reka t≈urebe\n");
    SDL_Delay(30);
}

bool generate_maze(maze_state_t *maze_state)
{
    sdl_on_key((key_handler_t)on_key);

    state_t *state = malloc(sizeof(state_t));
    // printf("Page: %d\n", state->page);

    init_grid(maze_state);

    // cell_t *cell = malloc(sizeof(cell_t));
    // cell->x = 1;
    // cell->y = 1;
    // visited[cell->x][cell->y] = true;

    // push_stack(&maze_state->head, cell);

    // printf("hano se \n");
    // while (maze_state->head != NULL)
    // {
    //     printf("hano se 1 \n");
    //     cell = pop_stack(&maze_state->head);
    //     printf("hano se 2 \n");

    //     if (get_neighbor(cell, maze_state->visited) != NULL)
    //     {
    //         printf("hano se 3 \n");

    //         push_stack(&maze_state->head, cell);
    //         printf("hano se 4 \n");

    //         cell_t *neighbor = get_neighbor(cell, maze_state->visited);
    //         printf("hano se 4 \n");

    //         remove_wall(cell, neighbor);
    //         printf("hano se 4 \n");

    //         maze_state->visited[neighbor->x][neighbor->y] = true;
    //         adjacency(cell, neighbor, maze_state->adj_matrix);
    //         push_stack(&maze_state->head, neighbor);
    //     }
    // }

    return false;
}
maze_state_t *maze_init()
{
    srand(time(NULL));

    maze_state_t *maze_state = malloc(sizeof(maze_state_t) + (sizeof(cell_t) * NUM_BUILDINGS));
    maze_state->head = NULL;

    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        size_t rand_x = (rand() % GRID_WIDTH) + 1;
        size_t rand_y = (rand() % GRID_HEIGHT) + 1;

        maze_state->buildings[i] = (cell_t){
            .x = ((GRID_WIDTH - rand_x) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
            .y = ((GRID_HEIGHT - rand_y) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
        };
        printf("Cell: (%zu, %zu)\n", rand_x, rand_y);
    }
    printf("aha ni sawa\n");
    init_maze(maze_state);

    printf("Visited sihit: %d\n", maze_state->adj_matrix[2][1]);
    printf("byapfuye\n");

    return maze_state;
}