#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"
#include "landing_page.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

struct state
{
    size_t page;
};

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
    if (state->page == 0)
    {
        state->page = build_landing_page(state->page);
        printf("stat of page: %zu", state->page);
    }
    sdl_show();
    return false;
}

void emscripten_free(state_t *state)
{
    asset_cache_destroy();
    free(state);
}
