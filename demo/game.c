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

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const vector_t SDL_MIN = {0, 0};
const vector_t SDL_MAX = {1000, 500};
const vector_t SDL_CENTER = {500, 250};

struct state
{
    scene_t *scene;
    size_t page;
    bool maze_generated;
};

void on_key(char key, key_event_type_t type, double held_time, state_t *state)
{
    if (type == KEY_PRESSED)
    {
        switch (key)
        {
        case LEFT_ARROW:
        {
            printf("going left\n");
            break;
        }
        case RIGHT_ARROW:
        {
            printf("going right\n");
            break;
        }
        case UP_ARROW:
        {
            printf("going up\n");
            break;
        }
        case DOWN_ARROW:
        {
            printf("going down\n");
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

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(SDL_MIN, SDL_MAX);
    state_t *state = malloc(sizeof(state_t));
    state->scene = scene_init();
    state->maze_generated = false;

    state->page = 1;

    return state;
}

bool emscripten_main(state_t *state)
{
    sdl_on_key((key_handler_t)on_key);

    if (state->page == 0)
    {
        build_landing_page();
    }
    else if (state->page == 1)
    {
        if (!state->maze_generated)
        {
            state->maze_generated = generate_maze(state);
        }
        sdl_show();
    }

    return false;
}

void emscripten_free(state_t *state)
{
    scene_free(state->scene);
    asset_cache_destroy();
    free(state);
}
