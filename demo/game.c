#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "landing_page.h"
#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

const vector_t SDL_MIN = {0, 0};
const vector_t SDL_MAX = {1000, 500};
const vector_t SDL_CENTER = {500, 250};

typedef struct cell
{
    size_t x;
    size_t y;
} cell_t;

typedef struct _stack
{
    cell_t *cell;
    struct _stack *next;
} stack_t;

const int grid_cell_size = 40;
const int grid_width = 25;
const int grid_height = 12;
const int window_width = (grid_width * grid_cell_size) + 1;
const int window_height = (grid_height * grid_cell_size) + 1;
const int number_of_cells = grid_width * grid_height;

struct state
{
    size_t page;
};

int findMax(int a, int b)
{
    return (a > b) ? a : b;
}

void init_grid()
{
    // draw_color((rgb_color_t){22, 22, 22});
    draw_color((rgb_color_t){210, 210, 210});

    for (int x = 0; x < window_width; x += grid_cell_size)
    {
        render_line(x, 0, x, window_height);
    }
    for (int y = 0; y < window_height; y += grid_cell_size)
    {
        render_line(0, y, window_width, y);
    }

    SDL_Rect start_cell = {(grid_cell_size / 4), (grid_cell_size / 4), (grid_cell_size / 2), (grid_cell_size / 2)};
    draw_color((rgb_color_t){0, 0, 0});

    render_rect(&start_cell);

    SDL_Rect terminal_cell;
    terminal_cell.x = ((grid_width - 1) * grid_cell_size) + grid_cell_size / 4;
    terminal_cell.y = ((grid_height - 1) * grid_cell_size) + grid_cell_size / 4;
    terminal_cell.w = grid_cell_size / 2;
    terminal_cell.h = grid_cell_size / 2;

    render_rect(&terminal_cell);
}

static stack_t *first_cell;
bool visited[grid_width + 2][grid_height + 2];
bool adjacency_matrx[number_of_cells][number_of_cells];

void init_maze()
{
    for (int i = 1; i <= grid_width; i++)
    {
        for (int j = 1; j <= grid_height; j++)
        {
            visited[i][j] = 0;
        }
    }
    for (int j = 1; j < grid_height + 2; j++)
    {
        visited[0][j] = true;
        visited[grid_width + 1][j] = true;
    }

    for (int i = 1; i <= grid_width; i++)
    {
        visited[i][0] = true;
        visited[i][grid_height + 1] = true;
    }
    for (int i = 0; i < number_of_cells; i++)
    {
        for (int j = 0; j < number_of_cells; j++)
        {
            adjacency_matrx[i][j] = false;
        }
    }
    first_cell = NULL;
    srand(time(NULL));
}

void push_stack(cell_t *cell)
{
    stack_t *s = malloc((sizeof(stack_t)));
    s->cell = cell;
    s->next = first_cell;
    first_cell = s;
}

cell_t *pop_stack()
{
    cell_t *removed = first_cell->cell;
    stack_t *p = first_cell;
    first_cell = first_cell->next;
    free(p);
    return removed;
}
void adjacency(cell_t *cell, cell_t *cellNeighbour)
{
    int cellPos = (cell->x - 1) * grid_height + (cell->y - 1);
    int neighbourPos = (cellNeighbour->x - 1) * grid_height + (cellNeighbour->y - 1);
    adjacency_matrx[cellPos][neighbourPos] = true;
    adjacency_matrx[neighbourPos][cellPos] = true;
}

bool isAdjacency(cell_t *cell, cell_t *cellNeighbour)
{
    int cellPos = (cell->x - 1) * grid_height + (cell->y - 1);
    int neighbourPos = (cellNeighbour->x - 1) * grid_height + (cellNeighbour->y - 1);
    return adjacency_matrx[cellPos][neighbourPos];
}

cell_t *vecNeighbor(cell_t *cell)
{
    cell_t *neighbor_vec = malloc(sizeof(cell_t));
    cell_t *null_vec = NULL;

    if ((visited[cell->x - 1][cell->y] == true) &&
        (visited[cell->x][cell->y - 1] == true) &&
        (visited[cell->x][cell->y + 1] == true) &&
        (visited[cell->x + 1][cell->y] == true))
        return null_vec;

    do
    {
        int randomNeighbour = (rand() % 4) + 1;
        switch (randomNeighbour)
        {
        case 1:
            neighbor_vec->x = cell->x - 1;
            neighbor_vec->y = cell->y;
            break;
        case 2:
            neighbor_vec->x = cell->x;
            neighbor_vec->y = cell->y - 1;
            break;
        case 3:
            neighbor_vec->x = cell->x;
            neighbor_vec->y = cell->y + 1;
            break;
        case 4:
            neighbor_vec->x = cell->x + 1;
            neighbor_vec->y = cell->y;
            break;
        }
    } while (visited[neighbor_vec->x][neighbor_vec->y] == true);

    return neighbor_vec;
}

void removeWall(cell_t *cell, cell_t *neighbour)
{
    if (cell->x == neighbour->x)
    {
        draw_color((rgb_color_t){22, 22, 22});
        int y = findMax(cell->y, neighbour->y);
        render_line((cell->x - 1) * grid_cell_size,
                    (y - 1) * grid_cell_size,
                    (cell->x - 1) * grid_cell_size + grid_cell_size,
                    (y - 1) * grid_cell_size);
    }

    else if (cell->y == neighbour->y)
    {
        draw_color((rgb_color_t){22, 22, 22});

        int x = findMax(cell->x, neighbour->x);
        render_line((x - 1) * grid_cell_size,
                    (cell->y - 1) * grid_cell_size,
                    (x - 1) * grid_cell_size,
                    (cell->y - 1) * grid_cell_size + grid_cell_size);
    }
    SDL_Delay(30);
    // sdl_show();
    // show();
}

void generate_maze()
{
    bool first_look = false;
    bool generate = false;

    init_maze();

    cell_t *cell = malloc(sizeof(cell_t));
    cell->x = 1;
    cell->y = 1;
    visited[cell->x][cell->y] = true;
    push_stack(cell);
    while (first_cell != NULL)
    {
        cell = pop_stack();
        if (vecNeighbor(cell) != NULL)
        {
            push_stack(cell);
            cell_t *neighbor = vecNeighbor(cell);
            removeWall(cell, neighbor);
            visited[neighbor->x][neighbor->y] = true;
            adjacency(cell, neighbor);
            push_stack(neighbor);
        }
    }
    // solve = SDL_TRUE;
    // generate = SDL_TRUE;
}

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(SDL_MIN, SDL_MAX);
    state_t *state = malloc(sizeof(state_t));
    state->page = 0;

    return state;
}

bool emscripten_main(state_t *state)
{
    sdl_clear();
    init_grid();
    generate_maze();
    sdl_show();
    return false;
}

void emscripten_free(state_t *state)
{
    asset_cache_destroy();
    free(state);
}
