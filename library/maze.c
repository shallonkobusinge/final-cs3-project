#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <emscripten/emscripten.h>

#define WIDTH 800
#define HEIGHT 800
#define CELL_SIZE 40
#define ROWS (HEIGHT / CELL_SIZE)
#define COLS (WIDTH / CELL_SIZE)

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

typedef struct
{
    int x, y;
} Cell;

int maze[ROWS][COLS];

void init_maze()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            maze[i][j] = 1;
        }
    }
}

void generate_maze(int x, int y)
{
    int dirs[] = {0, 1, 2, 3};
    for (int i = 0; i < 4; i++)
    {
        int r = rand() % 4;
        int temp = dirs[r];
        dirs[r] = dirs[i];
        dirs[i] = temp;
    }
    for (int i = 0; i < 4; i++)
    {
        int nx = x, ny = y;
        switch (dirs[i])
        {
        case 0:
            nx = x - 2;
            break;
        case 1:
            ny = y + 2;
            break;
        case 2:
            nx = x + 2;
            break;
        case 3:
            ny = y - 2;
            break;
        }
        if (nx > 0 && nx < ROWS - 1 && ny > 0 && ny < COLS - 1 && maze[nx][ny] == 1)
        {
            maze[nx][ny] = 0;
            maze[x + (nx - x) / 2][y + (ny - y) / 2] = 0;
            generate_maze(nx, ny);
        }
    }
}

void draw_maze()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < ROWS; ++i)
    {
        for (int j = 0; j < COLS; ++j)
        {
            if (maze[i][j] == 1)
            {
                SDL_Rect rect = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void main_loop()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            emscripten_cancel_main_loop();
            return;
        }
    }

    draw_maze();
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Maze", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        SDL_DestroyWindow(window);
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    srand(time(NULL));
    init_maze();
    maze[1][1] = 0;
    generate_maze(1, 1);

    emscripten_set_main_loop(main_loop, 0, 1);

    return 0;
}
