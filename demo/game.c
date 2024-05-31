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

const vector_t SDL_MIN = {0, 0};
const vector_t SDL_MAX = {1000, 500};
const vector_t SDL_CENTER = {500, 250};

struct state
{
    size_t page;
};

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(SDL_MIN, SDL_MAX);
    state_t *state = malloc(sizeof(state_t));
    state->page = 0;

    init_grid();
    sdl_show();

    return state;
}

bool emscripten_main(state_t *state)
{
    sdl_clear();
    thread = SDL_CreateThread(generate_maze, "Generating", NULL);

    return false;
}

void emscripten_free(state_t *state)
{
    asset_cache_destroy();
    free(state);
}
