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

    return state;
}

SDL_bool generate;
SDL_bool solve;
SDL_bool firstLook;

bool emscripten_main(state_t *state)
{
    sdl_clear();

    SDL_bool quit = SDL_FALSE;
    generate = SDL_TRUE;
    solve = SDL_FALSE;
    firstLook = SDL_TRUE;
    SDL_Thread *thread;

    init_grid();

    while (!quit)
    {
        SDL_Event event;
        while (SDL_WaitEvent(&event) && !quit)
        {
            if (firstLook)
            {
                init_grid();
                thread = SDL_CreateThread(generate_maze, "Generating", NULL);
            }
            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button)
                {
                case SDL_BUTTON_RIGHT:
                    if (generate)
                    {
                        init_grid();
                        thread = SDL_CreateThread(generate_maze, "Generating", NULL);
                    }
                    continue;
                case SDL_BUTTON_LEFT:
                    // if (solve)
                    //     thread = SDL_CreateThread(mazeSolving, "Solving", NULL);
                    continue;
                }
            case SDL_QUIT:
                quit = SDL_TRUE;
                break;
            }
        }

        // SDL_RenderPresent(renderer);
        sdl_show();
    }

    // SDL_Thread *thread = (generate_maze, "Generating", NULL);

    // generate_maze(NULL);
    // sdl_show();
    return false;
}

void emscripten_free(state_t *state)
{
    asset_cache_destroy();
    free(state);
}
