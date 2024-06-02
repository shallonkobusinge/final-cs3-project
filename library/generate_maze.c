#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cell.h"
#include "generate_maze.h"
#include "stack.h"
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

static stack_t *head;

/**
 * Initialize and draw the Maze Grid.
 */
void init_grid()
{
    sdl_clear();
    render_color((rgb_color_t){210, 210, 210});

    for (int x = 0; x < window_width; x += GRID_CELL_SIZE)
    {
        render_line(x, 0, x, window_height);
    }
    for (int y = 0; y < window_height; y += GRID_CELL_SIZE)
    {
        render_line(0, y, window_width, y);
    }

    SDL_Rect start_cell = {(GRID_CELL_SIZE / 10), (GRID_CELL_SIZE / 4), (GRID_CELL_SIZE / 2), (GRID_CELL_SIZE / 2)};
    render_color((rgb_color_t){0, 0, 0});
    render_rect(&start_cell);

    SDL_Rect terminal_cell;
    terminal_cell.x = ((GRID_WIDTH - 5) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4;
    terminal_cell.y = ((GRID_HEIGHT - 5) * GRID_CELL_SIZE) + GRID_CELL_SIZE / 4;
    terminal_cell.w = GRID_CELL_SIZE / 2;
    terminal_cell.h = GRID_CELL_SIZE / 2;
    render_rect(&terminal_cell);

    sdl_show();
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
            visited[i][j] = false;
        }
    }

    for (int i = 1; i <= GRID_WIDTH; i++)
    {
        visited[i][0] = true;
        visited[i][GRID_HEIGHT + 1] = true;
    }

    for (int j = 1; j < GRID_HEIGHT + 2; j++)
    {
        visited[0][j] = true;
        visited[GRID_WIDTH + 1][j] = true;
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

/**
 * Removes the wall between the current cell and its neighbor.
 * Checks whether the neighbor is in the same row or column and draws a line to remove the wall.
 * @param cell current cell
 * @param neighbor cell neighbor
 */
static void remove_wall(cell_t *cell, cell_t *neighbor)
{
    printf("hase\n");

    if (cell->x == neighbor->x)
    {
        render_color((rgb_color_t){22, 22, 22});

        int pos_y = (cell->y > neighbor->y) ? cell->y : neighbor->y;

        render_line((cell->x - 1) * GRID_CELL_SIZE,
                    (pos_y - 1) * GRID_CELL_SIZE,
                    (cell->x - 1) * GRID_CELL_SIZE + GRID_CELL_SIZE,
                    (pos_y - 1) * GRID_CELL_SIZE);
    }
    else if (cell->y == neighbor->y)
    {
        render_color((rgb_color_t){22, 22, 22});

        int pos_x = (cell->x > neighbor->x) ? cell->x : neighbor->x;

        render_line((pos_x - 1) * GRID_CELL_SIZE,
                    (cell->y - 1) * GRID_CELL_SIZE,
                    (pos_x - 1) * GRID_CELL_SIZE,
                    (cell->y - 1) * GRID_CELL_SIZE + GRID_CELL_SIZE);
    }
    printf("ehg ?\n");
    render_show();
    SDL_Delay(30);
}

int generate_maze(void *ptr)
{
    init_maze();

    cell_t *cell = malloc(sizeof(cell_t));
    cell->x = 1;
    cell->y = 1;
    visited[cell->x][cell->y] = true;

    push_stack(head, cell);
    while (head != NULL)
    {
        cell = pop_stack(head);
        if (get_neighbor(cell, visited) != NULL)
        {
            push_stack(head, cell);
            cell_t *neighbor = get_neighbor(cell, visited);
            remove_wall(cell, neighbor);
            visited[neighbor->x][neighbor->y] = true;
            adjacency(cell, neighbor, adj_matrix);
            push_stack(head, neighbor);
        }
    }
}