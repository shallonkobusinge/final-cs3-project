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

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

#define WIDTH 800
#define HEIGHT 800
#define CELL_SIZE 20

typedef struct
{
    int x, y;
} Cell;

struct state
{
    size_t page;
};

void draw_maze()
{
    // Draw the maze walls (hardcoded example).
    // You can replace this with your maze generation algorithm.

    int maze[20][20] = {/* Your maze array here */};

    for (int i = 0; i < 20; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            if (maze[i][j] == 1)
            {
                SDL_Rect rect = {j * 200, i * 200, 200, 200};
                        }
        }
    }
}

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(MIN, MAX);
    state_t *state = malloc(sizeof(state_t));
    state->page = 0;

    return state;
}

bool emscripten_main(state_t *state)
{
    sdl_clear();
    // if (state->page == 0)
    // {
    draw_maze();
    // build_landing_p2age();
    // }
    sdl_show();
    return false;
}

void emscripten_free(state_t *state)
{
    asset_cache_destroy();
    free(state);
}
