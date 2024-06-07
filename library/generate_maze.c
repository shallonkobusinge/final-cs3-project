#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cell.h"
#include "generate_maze.h"
#include "stack.h"
#include "state.h"
#include "sdl_wrapper.h"

const size_t GRID_WIDTH = 5;
const size_t GRID_HEIGHT = 5;
const size_t NUM_CELLS = GRID_WIDTH * GRID_HEIGHT;

const int GRID_CELL_SIZE = 40;
const int window_width = (GRID_WIDTH * GRID_CELL_SIZE) + 1;
const int window_height = (GRID_HEIGHT * GRID_CELL_SIZE) + 1;

bool visited[GRID_WIDTH + 2][GRID_HEIGHT + 2];
bool adj_matrix[NUM_CELLS][NUM_CELLS];
cell_t *parent[GRID_WIDTH][GRID_HEIGHT];

SDL_Rect hider_cell = (SDL_Rect){(GRID_CELL_SIZE / 4), (GRID_CELL_SIZE / 4), (GRID_CELL_SIZE / 2), (GRID_CELL_SIZE / 2)};

typedef struct
{
    SDL_Rect box;
    bool left, top, right, bottom;
    bool visited, start, end;
} TCell;

typedef struct Node
{
    int x, y;
    struct Node *next;
} Node;

typedef struct
{
    int width, height, cell_size;
    TCell cells[GRID_HEIGHT][GRID_WIDTH];
    Node *stack;
} Maze;

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

typedef struct
{
    SDL_Rect box;
    bool left, top, right, bottom;
    bool visited, start, end;
} TCell;

typedef struct
{
    int width, height, cell_size;
    TCell cells[GRID_HEIGHT][GRID_WIDTH];
} Maze;

const size_t NUM_BUILDINGS = 1;
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

void init_maze_d(Maze *maze, int width, int height, int cell_size)
{
    maze->width = width;
    maze->height = height;
    maze->cell_size = cell_size;
    maze->stack = NULL;

    for (int y = 0; y < GRID_HEIGHT; ++y)
    {
        for (int x = 0; x < GRID_WIDTH; ++x)
        {
            maze->cells[y][x] = (TCell){
                .box = {x * cell_size, y * cell_size, cell_size, cell_size},
                .left = true,
                .top = true,
                .right = true,
                .bottom = true,
                .visited = false,
                .start = false,
                .end = false};
        }
    }
}

void push(Node **stack, int x, int y)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->x = x;
    node->y = y;
    node->next = *stack;
    *stack = node;
}

void pop(Node **stack, int *x, int *y)
{
    Node *top = *stack;
    *x = top->x;
    *y = top->y;
    *stack = top->next;
    free(top);
}

bool is_empty(Node *stack)
{
    return stack == NULL;
}

void remove_wall(TCell *current, TCell *next, int direction)
{
    switch (direction)
    {
    case 0:
        current->top = false;
        next->bottom = false;
        break; // Up
    case 1:
        current->right = false;
        next->left = false;
        break; // Right
    case 2:
        current->bottom = false;
        next->top = false;
        break; // Down
    case 3:
        current->left = false;
        next->right = false;
        break; // Left
    }
}

int get_neighbors(Maze *maze, int x, int y, int neighbors[][2])
{
    int count = 0;

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
            maze_state->adj_matrix[i][j] = false;
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

void init_mazeS(Maze *maze, int width, int height, int cell_size)
{
    maze->width = width;
    maze->height = height;
    maze->cell_size = cell_size;
    for (int y = 0; y < GRID_HEIGHT; ++y)
    {
        for (int x = 0; x < GRID_WIDTH; ++x)
        {
            maze->cells[y][x] = (TCell){
                .box = {x * cell_size, y * cell_size, cell_size, cell_size},
                .left = true,
                .top = true,
                .right = true,
                .bottom = true,
                .visited = false,
                .start = false,
                .end = false};
        }
    }
}

void apply_aldous_broder(Maze *maze)
{
    int total_cells = GRID_WIDTH * GRID_HEIGHT;
    int visited_cells = 1;
    int x = rand() % GRID_WIDTH;
    int y = rand() % GRID_HEIGHT;

    maze->cells[y][x].visited = true;

    while (visited_cells < total_cells)
    {
        int direction = rand() % 4;
        int nx = x, ny = y;

        switch (direction)
        {
        case 0:
            if (y > 0)
                ny--;
            break; // Up
        case 1:
            if (x < GRID_WIDTH - 1)
                nx++;
            break; // Right
        case 2:
            if (y < GRID_HEIGHT - 1)
                ny++;
            break; // Down
        case 3:
            if (x > 0)
                nx--;
            break; // Left
        }

        if (maze->cells[ny][nx].visited == false)
        {
            maze->cells[ny][nx].visited = true;
            visited_cells++;

            if (ny == y - 1)
            {
                maze->cells[y][x].top = false;
                maze->cells[ny][nx].bottom = false;
            }
            if (nx == x + 1)
            {
                maze->cells[y][x].right = false;
                maze->cells[ny][nx].left = false;
            }
            if (ny == y + 1)
            {
                maze->cells[y][x].bottom = false;
                maze->cells[ny][nx].top = false;
            }
            if (nx == x - 1)
            {
                maze->cells[y][x].left = false;
                maze->cells[ny][nx].right = false;
            }
        }

        x = nx;
        y = ny;
    }
}

void draw_maze(Maze *maze)
{
    render_color((rgb_color_t){255, 0, 0});

    for (int y = 0; y < GRID_HEIGHT; ++y)
    {
        for (int x = 0; x < GRID_WIDTH; ++x)
        {
            TCell *cell = &maze->cells[y][x];
            if (cell->top)
                render_line(cell->box.x, cell->box.y, cell->box.x + cell->box.w, cell->box.y);
            if (cell->right)
                render_line(cell->box.x + cell->box.w, cell->box.y, cell->box.x + cell->box.w, cell->box.y + cell->box.h);
            if (cell->bottom)
                render_line(cell->box.x, cell->box.y + cell->box.h, cell->box.x + cell->box.w, cell->box.y + cell->box.h);
            if (cell->left)
                render_line(cell->box.x, cell->box.y, cell->box.x, cell->box.y + cell->box.h);
        }
    }
}

void generate_maze(maze_state_t *maze_state)
{
    Maze maze;
    init_maze(&maze, GRID_WIDTH, GRID_HEIGHT, GRID_CELL_SIZE);

    init_grid(maze_state);
    int x = 0, y = 0;
    maze->cells[y][x].visited = true;
    push(&maze->stack, x, y);

    while (!is_empty(maze->stack))
    {
        int neighbors[4][2];
        int count = get_neighbors(maze, x, y, neighbors);

        if (count > 0)
        {
            int r = rand() % count;
            int nx = neighbors[r][0];
            int ny = neighbors[r][1];
            int direction = (nx == x) ? ((ny > y) ? 2 : 0) : ((nx > x) ? 1 : 3);

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

void draw_maze(Maze *maze)
{
    render_color((rgb_color_t){255, 255, 255});

    for (int y = 0; y < GRID_HEIGHT; ++y)
    {
        for (int x = 0; x < GRID_WIDTH; ++x)
        {
            TCell *cell = &maze->cells[y][x];
            if (cell->top)
                render_line(cell->box.x, cell->box.y, cell->box.x + cell->box.w, cell->box.y);
            if (cell->right)
                render_line(cell->box.x + cell->box.w, cell->box.y, cell->box.x + cell->box.w, cell->box.y + cell->box.h);
            if (cell->bottom)
                render_line(cell->box.x, cell->box.y + cell->box.h, cell->box.x + cell->box.w, cell->box.y + cell->box.h);
            if (cell->left)
                render_line(cell->box.x, cell->box.y, cell->box.x, cell->box.y + cell->box.h);
        }
    }
}

// bool generate_maze(maze_state_t *maze_state)
// {
//     sdl_on_key((key_handler_t)on_key);

//     init_grid(maze_state);

//     cell_t *cell = malloc(sizeof(cell_t));

//     Maze maze;
//     init_mazeS(&maze, GRID_WIDTH, GRID_HEIGHT, GRID_CELL_SIZE);
//     apply_aldous_broder(&maze);

//     draw_maze(&maze);
// cell->x = 1;
// cell->y = 1;
// maze_state->visited[cell->x][cell->y] = true;

// push_stack(&maze_state->head, cell);

// while (maze_state->head != NULL)
// {
//     cell = pop_stack(&maze_state->head);

//     cell_t *neighbor = get_neighbor(cell, maze_state->visited);

//     if (neighbor != NULL)
//     {

//         // push_stack(&maze_state->head, cell);

//         cell_t *neighbor = get_neighbor(cell, maze_state->visited);

//         remove_wall(cell, neighbor);

//         maze_state->visited[neighbor->x][neighbor->y] = true;
//         adjacency(cell, neighbor, maze_state->adj_matrix);
//         push_stack(&maze_state->head, neighbor);
// }
// }

// if (neighbor != NULL)
// {
//     push_stack(&maze_state->head, cell); // Push the current cell back
//     remove_wall(cell, neighbor);
//     maze_state->visited[neighbor->x][neighbor->y] = true;
//     adjacency(cell, neighbor, maze_state->adj_matrix);
//     push_stack(&maze_state->head, neighbor);
// }
// else
// {
//     free(cell); // Free the cell if no unvisited neighbors
// }
// }
// }

// return false;
// }

maze_state_t *maze_init()
{
    // srand(time(NULL));

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