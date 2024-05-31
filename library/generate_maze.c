#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cell.h"
#include "generate_maze.h"
#include "stack.h"
#include "sdl_wrapper.h"

const int grid_cell_size = 40;
const int window_width = (grid_width * grid_cell_size) + 1;
const int window_height = (grid_height * grid_cell_size) + 1;

bool visited[grid_width + 2][grid_height + 2];
bool adj_matrix[num_cells][num_cells];
cell_t *parent[grid_width][grid_height];

static stack_t *head;

/**
 * Initialize and draw the Maze Grid.
 */
static void init_grid()
{
    render_color((rgb_color_t){210, 210, 210});

    for (int x = 0; x < window_width; x += grid_cell_size)
    {
        render_line(x, 0, x, window_height);
    }
    for (int y = 0; y < window_height; y += grid_cell_size)
    {
        render_line(0, y, window_width, y);
    }

    SDL_Rect start_cell = {(grid_cell_size / 4), (grid_cell_size / 4), (grid_cell_size / 2), (grid_cell_size / 2)};
    render_color((rgb_color_t){0, 0, 0});
    render_rect(&start_cell);

    SDL_Rect terminal_cell;
    terminal_cell.x = ((grid_width - 1) * grid_cell_size) + grid_cell_size / 4;
    terminal_cell.y = ((grid_height - 1) * grid_cell_size) + grid_cell_size / 4;
    terminal_cell.w = grid_cell_size / 2;
    terminal_cell.h = grid_cell_size / 2;
    render_rect(&terminal_cell);
}

/**
 * Initializes the maze by marking all cells as unvisited and setting the border cells as visited.
 * and initializes the adjacency matrix and sets the random seed.
 */
static void init_maze()
{
    for (int i = 1; i <= grid_width; i++)
    {
        for (int j = 1; j <= grid_height; j++)
        {
            visited[i][j] = false;
        }
    }

    for (int i = 1; i <= grid_width; i++)
    {
        visited[i][0] = true;
        visited[i][grid_height + 1] = true;
    }

    for (int j = 1; j < grid_height + 2; j++)
    {
        visited[0][j] = true;
        visited[grid_width + 1][j] = true;
    }

    for (int i = 0; i < num_cells; i++)
    {
        for (int j = 0; j < num_cells; j++)
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
    render_color((rgb_color_t){22, 22, 22});

    if (cell->x == neighbor->x)
    {
        int pos_y = (cell->y > neighbor->y) ? cell->y : neighbor->y;

        render_line((cell->x - 1) * grid_cell_size,
                    (pos_y - 1) * grid_cell_size,
                    (cell->x - 1) * grid_cell_size + grid_cell_size,
                    (pos_y - 1) * grid_cell_size);
    }
    else if (cell->y == neighbor->y)
    {
        int pos_x = (cell->x > neighbor->x) ? cell->x : neighbor->x;

        render_line((pos_x - 1) * grid_cell_size,
                    (cell->y - 1) * grid_cell_size,
                    (pos_x - 1) * grid_cell_size,
                    (cell->y - 1) * grid_cell_size + grid_cell_size);
    }
    SDL_Delay(30);
}

void generate_maze()
{
    init_grid();
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
            cell_t *neighbor = get_neighbor(cell, head);
            remove_wall(cell, neighbor);
            visited[neighbor->x][neighbor->y] = true;
            adjacency(cell, neighbor);
            push_stack(head, neighbor);
        }
    }
}