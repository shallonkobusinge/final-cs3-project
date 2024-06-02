#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "landing_page.h"
// #include "generate_maze.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const vector_t SDL_MIN = {0, 0};
const vector_t SDL_MAX = {1000, 500};
const vector_t SDL_CENTER = {500, 250};

#define WIDTH 1000
#define HEIGHT 800
#define CELL_SIZE 20

static int once = 0;
typedef struct
{
    int x, y;
} Cell;

typedef struct
{
    Cell *cells;
    int size;
    int capacity;
} Stack;

void push(Stack *stack, Cell cell)
{
    if (stack->size >= stack->capacity)
    {
        stack->capacity *= 2;
        stack->cells = realloc(stack->cells, stack->capacity * sizeof(Cell));
    }
    stack->cells[stack->size++] = cell;
}

Cell pop(Stack *stack)
{
    return stack->cells[--stack->size];
}

bool is_empty(Stack *stack)
{
    return stack->size == 0;
}

void draw_cell(Cell cell, int r, int g, int b)
{
    render_color((rgb_color_t){
        r,
        g, b});
    SDL_Rect rect = {cell.x * CELL_SIZE, cell.y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    render_rect(&rect);
    sdl_show();
}

void draw_maze(bool **visited)
{
    render_color((rgb_color_t){
        255, 255, 255});
    for (int x = 0; x < WIDTH / CELL_SIZE; x++)
    {
        for (int y = 0; y < HEIGHT / CELL_SIZE; y++)
        {
            if (!visited[x][y])
            {
                SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                render_rect(&rect);
                sdl_show();
            }
        }
    }
}

void connect_cells(Cell a, Cell b)
{
    render_color((rgb_color_t){
        0, 0, 0});
    render_line(a.x * CELL_SIZE + CELL_SIZE / 2, a.y * CELL_SIZE + CELL_SIZE / 2,
                b.x * CELL_SIZE + CELL_SIZE / 2, b.y * CELL_SIZE + CELL_SIZE / 2);
    sdl_show();
}

Cell get_random_neighbor(Cell cell, bool **visited)
{
    Cell neighbors[4];
    int count = 0;

    if (cell.x > 0 && !visited[cell.x - 1][cell.y])
    {
        neighbors[count++] = (Cell){cell.x - 1, cell.y};
    }
    if (cell.x < WIDTH / CELL_SIZE - 1 && !visited[cell.x + 1][cell.y])
    {
        neighbors[count++] = (Cell){cell.x + 1, cell.y};
    }
    if (cell.y > 0 && !visited[cell.x][cell.y - 1])
    {
        neighbors[count++] = (Cell){cell.x, cell.y - 1};
    }
    if (cell.y < HEIGHT / CELL_SIZE - 1 && !visited[cell.x][cell.y + 1])
    {
        neighbors[count++] = (Cell){cell.x, cell.y + 1};
    }

    if (count == 0)
    {
        return (Cell){-1, -1};
    }

    return neighbors[rand() % count];
}

void randomized_dfs(bool **visited)
{
    Stack stack;
    stack.size = 0;
    stack.capacity = 16;
    stack.cells = malloc(stack.capacity * sizeof(Cell));

    Cell start = {0, 0};
    push(&stack, start);
    visited[start.x][start.y] = true;

    while (!is_empty(&stack))
    {
        Cell current = pop(&stack);
        draw_cell(current, 0, 0, 0);

        Cell next;
        do
        {
            next = get_random_neighbor(current, visited);
            if (next.x != -1)
            {
                visited[next.x][next.y] = true;
                connect_cells(current, next);
                push(&stack, next);
                push(&stack, current);
            }
        } while (next.x != -1);
    }

    free(stack.cells);
}

struct state
{
    scene_t *scene;
    size_t page;
};

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

int findMax(int a, int b)
{
    return (a > b) ? a : b;
}
int findMin(int a, int b)
{
    return (a > b) ? b : a;
}

static stack_t *first_cell;
bool visited[grid_width + 2][grid_height + 2];
bool adjacency_matrx[number_of_cells][number_of_cells];
cell_t *parent[grid_width][grid_height];

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

    printf("Adjacnence %d\n", adjacency_matrx[10][10]);
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
    if (once == 0)
    {
        printf("removing 1\n");
        if (cell->x == neighbour->x)
        {
            render_color((rgb_color_t){255, 0, 0});
            int y = findMax(cell->y, neighbour->y);
            render_line((cell->x - 1) * grid_cell_size,
                        (y - 1) * grid_cell_size,
                        (cell->x - 1) * grid_cell_size + grid_cell_size,
                        (y - 1) * grid_cell_size);
        }

        else if (cell->y == neighbour->y)
        {
            render_color((rgb_color_t){255, 0, 0});

            int x = findMax(cell->x, neighbour->x);
            render_line((x - 1) * grid_cell_size,
                        (cell->y - 1) * grid_cell_size,
                        (x - 1) * grid_cell_size,
                        (cell->y - 1) * grid_cell_size + grid_cell_size);
        }
        // SDL_Delay(30);
        sdl_show();
        // show();
        once = 1;
    }
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
}

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(SDL_MIN, SDL_MAX);
    state_t *state = malloc(sizeof(state_t));
    state->scene = scene_init();

    state->page = 1;

    return state;
}

bool emscripten_main(state_t *state)
{
    sdl_clear();
    if (state->page == 0)
    {
        build_landing_page();
    }
    else if (state->page == 1)
    {
        // init_grid();
        generate_maze();
    }

    sdl_show();

    return false;
}

void emscripten_free(state_t *state)
{
    scene_free(state->scene);
    asset_cache_destroy();
    free(state);
}
