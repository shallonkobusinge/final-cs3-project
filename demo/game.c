#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "landing_page.h"
#include "generate_maze.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"
#include "stack.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const vector_t SDL_MIN = {0, 0};
const vector_t SDL_MAX = {1000, 500};
const vector_t SDL_CENTER = {500, 250};

typedef struct maze_state maze_state_t;
typedef struct landing_page_state landing_page_state_t;

struct state
{
    scene_t *scene;
    size_t page;
    maze_state_t *maze_state;
    landing_page_state_t *landing_page_state;
};

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(SDL_MIN, SDL_MAX);

    state_t *state = malloc(sizeof(state_t));
    state->scene = scene_init();
    state->page = 1;
    state->maze_state = maze_init();
    state->landing_page_state = maze_init();

    return state;
}

bool emscripten_main(state_t *state)
{
    sdl_clear();

    if (state->page == 0)
    {
        show_landing_page();
    }
    else if (state->page == 1)
    {
        show_maze(state->maze_state);
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
