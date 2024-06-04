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
static stack_t *head;
typedef struct state
{
    scene_t *scene;
    size_t page;
    bool maze_generated;
    list_t *seekers;
} state_t;

const size_t NUM_BUILDINGS = 4;

cell_t buildings[] = {
    {
        .x = ((GRID_WIDTH - 3) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
        .y = ((GRID_HEIGHT - 3) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
    },
    {
        .x = ((GRID_WIDTH - 12) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
        .y = ((GRID_HEIGHT - 12) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
    },
    {
        .x = ((GRID_WIDTH - 10) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
        .y = ((GRID_HEIGHT - 5) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
    },
    {
        .x = ((GRID_WIDTH - 24) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
        .y = ((GRID_HEIGHT - 2) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
    },
};
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

/*
 * Moves the seeker cell to a random
 * adjacent cell 
*/
static void random_move_seeker (SDL_Rect *terminal_cell) {
    printf("WE are here ");
    SDL_Delay(70);
    int direction = rand() % 4;
    switch (direction) {
    case 0: { // move left
     if (terminal_cell->x - GRID_CELL_SIZE >= 0) {
            terminal_cell->x -= GRID_CELL_SIZE;
        }
        break;
    }
    case 1: { // move right
        if (terminal_cell->x + GRID_CELL_SIZE < window_width) {
            terminal_cell->x += GRID_CELL_SIZE;
        }
        break;
    }
    case 2: { // move up
        if (terminal_cell->y - GRID_CELL_SIZE >= 0) {
            terminal_cell->y -= GRID_CELL_SIZE;
            
        }
        break;
    }
    case 3: { // move down
        if (terminal_cell->y + GRID_CELL_SIZE < window_height) {
            terminal_cell->y += GRID_CELL_SIZE;
            
        }
        break;
    }
    default:
        break;
    }
}

/**
 * Initialize and draw the Maze Grid.
 */
static void init_grid(state_t *state)
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
        // SDL_Rect cell = {buildings[i].x, buildings[i].y, GRID_CELL_SIZE / 2, GRID_CELL_SIZE / 2};
        SDL_Rect *cell;
        cell->x = ((rand() % GRID_WIDTH) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
        cell->y = ((rand() % GRID_HEIGHT) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4,
        cell->w = GRID_CELL_SIZE / 2;
        cell->h = GRID_CELL_SIZE / 2;
        render_color((rgb_color_t){241, 108, 45});
        render_rect(&cell);
        list_add(state->seekers, cell);
    }
}

/**
 * Initializes the maze by marking all cells as unvisited and setting the border cells as visited.
 * and initializes the adjacency matrix and sets the random seed.
 */
static void init_maze()
{
    for (int i = 1; i <= GRID_WIDTH; i++)
    {
        for (int j = 1; j <= GRID_HEIGHT; j++)
        {
            visited[i][j] = 0;
        }
    }
    for (int j = 1; j < GRID_HEIGHT + 2; j++)
    {
        visited[0][j] = true;
        visited[GRID_WIDTH + 1][j] = true;
    }

    for (int i = 1; i <= GRID_WIDTH; i++)
    {
        visited[i][0] = true;
        visited[i][GRID_HEIGHT + 1] = true;
    }

    for (int i = 0; i < NUM_CELLS; i++)
    {
        for (int j = 0; j < NUM_CELLS; j++)
        {
            adj_matrix[i][j] = false;
        }
    }
    head = NULL;
    srand(time(NULL));
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
    SDL_Delay(30);
}

bool generate_maze(state_t *state)
{
    sdl_on_key((key_handler_t)on_key);

    printf("Page: %d\n", state->page);

    init_grid(state);
    init_maze();
    for(size_t i = 0; i < list_size(state->seekers); i++) {
        SDL_Rect *rect = list_get(state->seekers, i);
        random_move_seeker(rect);
    }
    //     random_move_seeker();
    // render_color((rgb_color_t){0, 0, 0});
    // render_rect(&terminal_cell);
    // // sdl_show();
    // return false;

    cell_t *cell = malloc(sizeof(cell_t));
    cell->x = 1;
    cell->y = 1;
    visited[cell->x][cell->y] = true;

    push_stack(&head, cell);

    while (head != NULL)
    {
        cell = pop_stack(&head);
        if (get_neighbor(cell, visited) != NULL)
        {
            push_stack(&head, cell);
            cell_t *neighbor = get_neighbor(cell, visited);
            remove_wall(cell, neighbor);
            visited[neighbor->x][neighbor->y] = true;
            adjacency(cell, neighbor, adj_matrix);
            push_stack(&head, neighbor);
        }
    }
    return false;
}
