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

int findMax(int a, int b)
{
    return (a > b) ? a : b;
}
int findMin(int a, int b)
{
    return (a > b) ? b : a;
}

// typedef struct stack
// {
//     cell_t *cell;
//     struct stack *next;
// } stack_t;

stack_t *head;

// void push_stack(cell_t *cell)
// {
//     stack_t *stack = malloc(sizeof(stack_t));
//     stack->cell = cell;
//     stack->next = head;
//     head = stack;
// }

// cell_t *pop_stack()
// {
//     cell_t *removed = head->cell;
//     stack_t *temp = head;
//     head = head->next;
//     free(temp);
//     return removed;
// }

/**
 * Initialize and draw the Maze Grid.
 */
void init_grid()
{
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
}

/**
 * Initializes the maze by marking all cells as unvisited and setting the border cells as visited.
 * and initializes the adjacency matrix and sets the random seed.
 */
void init_maze()
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

/**
 * Removes the wall between the current cell and its neighbor.
 * Checks whether the neighbor is in the same row or column and draws a line to remove the wall.
 * @param cell current cell
 * @param neighbor cell neighbor
 */
void remove_wall(cell_t *cell, cell_t *neighbor)
{
    printf("removing\n");
    if (cell->x == neighbor->x)
    {
        render_color((rgb_color_t){255, 0, 0});
        int y = findMax(cell->y, neighbor->y);
        render_line((cell->x - 1) * GRID_CELL_SIZE,
                    (y - 1) * GRID_CELL_SIZE,
                    (cell->x - 1) * GRID_CELL_SIZE + GRID_CELL_SIZE,
                    (y - 1) * GRID_CELL_SIZE);
    }

    else if (cell->y == neighbor->y)
    {
        render_color((rgb_color_t){255, 0, 0});

        int x = findMax(cell->x, neighbor->x);
        render_line((x - 1) * GRID_CELL_SIZE,
                    (cell->y - 1) * GRID_CELL_SIZE,
                    (x - 1) * GRID_CELL_SIZE,
                    (cell->y - 1) * GRID_CELL_SIZE + GRID_CELL_SIZE);
    }
    SDL_Delay(30);
}

// void adjacency(cell_t *cell, cell_t *cellNeighbour)
// {
//     int cellPos = (cell->x - 1) * GRID_HEIGHT + (cell->y - 1);
//     int neighbourPos = (cellNeighbour->x - 1) * GRID_HEIGHT + (cellNeighbour->y - 1);
//     adj_matrix[cellPos][neighbourPos] = true;
//     adj_matrix[neighbourPos][cellPos] = true;
// }

// bool isAdjacency(cell_t *cell, cell_t *cellNeighbour)
// {
//     int cellPos = (cell->x - 1) * GRID_HEIGHT + (cell->y - 1);
//     int neighbourPos = (cellNeighbour->x - 1) * GRID_HEIGHT + (cellNeighbour->y - 1);
//     return adj_matrix[cellPos][neighbourPos];
// }

// cell_t *vecNeighbor(cell_t *cell)
// {
//     cell_t *neighbor_vec = malloc(sizeof(cell_t));
//     cell_t *null_vec = NULL;

//     if ((visited[cell->x - 1][cell->y] == true) &&
//         (visited[cell->x][cell->y - 1] == true) &&
//         (visited[cell->x][cell->y + 1] == true) &&
//         (visited[cell->x + 1][cell->y] == true))
//         return null_vec;

//     do
//     {
//         int randomNeighbour = (rand() % 4) + 1;
//         switch (randomNeighbour)
//         {
//         case 1:
//             neighbor_vec->x = cell->x - 1;
//             neighbor_vec->y = cell->y;
//             break;
//         case 2:
//             neighbor_vec->x = cell->x;
//             neighbor_vec->y = cell->y - 1;
//             break;
//         case 3:
//             neighbor_vec->x = cell->x;
//             neighbor_vec->y = cell->y + 1;
//             break;
//         case 4:
//             neighbor_vec->x = cell->x + 1;
//             neighbor_vec->y = cell->y;
//             break;
//         }
//     } while (visited[neighbor_vec->x][neighbor_vec->y] == true);

//     return neighbor_vec;
// }

void generate_maze()
{
    printf("here 1\n");

    init_grid();
    printf("here 2\n");

    init_maze();
    printf("here 3\n");

    cell_t *cell = malloc(sizeof(cell_t));
    cell->x = 1;
    cell->y = 1;
    printf("here 4\n");

    visited[cell->x][cell->y] = true;

    printf("here 5\n");
    push_stack(head, cell);
    printf("here 6\n");

    while (head != NULL)
    {
        printf("fiurst cell aka head\n");

        printf("here 1\n");
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