#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 800
#define CELL_SIZE 20

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

void draw_cell(SDL_Renderer *renderer, Cell cell, int r, int g, int b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_Rect rect = {cell.x * CELL_SIZE, cell.y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &rect);
}

void draw_maze(SDL_Renderer *renderer, bool **visited)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (int x = 0; x < WIDTH / CELL_SIZE; x++)
    {
        for (int y = 0; y < HEIGHT / CELL_SIZE; y++)
        {
            if (!visited[x][y])
            {
                SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

void connect_cells(SDL_Renderer *renderer, Cell a, Cell b)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, a.x * CELL_SIZE + CELL_SIZE / 2, a.y * CELL_SIZE + CELL_SIZE / 2,
                       b.x * CELL_SIZE + CELL_SIZE / 2, b.y * CELL_SIZE + CELL_SIZE / 2);
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

void randomized_dfs(SDL_Renderer *renderer, bool **visited)
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
        draw_cell(renderer, current, 0, 0, 0);

        Cell next;
        do
        {
            next = get_random_neighbor(current, visited);
            if (next.x != -1)
            {
                visited[next.x][next.y] = true;
                connect_cells(renderer, current, next);
                push(&stack, next);
                push(&stack, current);
            }
        } while (next.x != -1);
    }

    free(stack.cells);
}
